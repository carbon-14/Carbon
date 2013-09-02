#include "BSPConverter.h"

#include "libxml.h"
#include "libxml/parser.h"

#include <vector>
#include <map>

#include <cassert>

#if defined( CARBON_PLATFORM_WIN32 )
#include <Windows.h>
#endif

bool RecursiveCreateDirectory( const char * dir )
{
#if defined( CARBON_PLATFORM_WIN32 )
    if ( ! CreateDirectory( dir, NULL ) )
    {
        DWORD err = GetLastError();
        if ( err == ERROR_ALREADY_EXISTS )
        {
            return true;
        }
        if ( err == ERROR_PATH_NOT_FOUND )
        {
            char * fileStart;
            char parentDir[ 256 ];
            if ( GetFullPathName( dir, 256, parentDir, &fileStart ) == 0 )
            {
                return false;
            }
            *(fileStart - 1 ) = 0;

            if ( RecursiveCreateDirectory( parentDir ) )
            {
                return ( CreateDirectory( dir, NULL ) != 0 );
            }

            return false;
        }

        return false;
    }
#endif
    return true;
}

bool BuildDirectory( const char * path )
{
#if defined( CARBON_PLATFORM_WIN32 )
    char dir[ 256 ];
    char * fileStart;
    if ( GetFullPathName( path, 256, dir, &fileStart ) == 0 )
    {
        return false;
    }
    *( fileStart - 1 ) = 0;

    if ( ! RecursiveCreateDirectory( dir ) )
    {
        return false;
    }
#endif
    return true;
}

void MakeStringID( const char * istr, char * ostr )
{
    for ( ; *istr != 0; ++istr, ++ostr )
    {
        if (    ( *istr > 0x2f && *istr < 0x3a )  // numbers
            ||  ( *istr > 0x40 && *istr < 0x5b )  // upper case alpha
            ||  ( *istr > 0x60 && *istr < 0x7b )  // lower case alpha
            )
        {
            *ostr = *istr;
        }
        else
        {
            *ostr = '_';
        }
    }

    *ostr = 0;
}

// BSP format found on :
// http://www.mralligator.com/q3/
// Thanks guys !

enum BSPLump
{
    LEntities = 0,  // 	Game-related object descriptions.
    LTextures,      // 	Surface descriptions.
    LPlanes,        // 	Planes used by map geometry.
    LNodes,         // 	BSP tree nodes.
    LLeafs,         // 	BSP tree leaves.
    LLeaffaces,     // 	Lists of face indices, one list per leaf.
    LLeafbrushes,   // 	Lists of brush indices, one list per leaf.
    LModels,        // 	Descriptions of rigid world geometry in map.
    LBrushes,       // 	Convex polyhedra used to describe solid space.
    LBrushsides,    // 	Brush surfaces.
    LVertices,      // 	Vertices used to describe faces.
    LMeshverts,     // 	Lists of offsets, one list per mesh.
    LEffects,       // 	List of special map effects.
    LFaces,         // 	Surface geometry.
    LLightmaps,     // 	Packed lightmap data.
    LLightvols,     // 	Local illumination data.
    LVisdata        // 	Cluster-cluster visibility data.
};

struct BSPDirEntry
{
    int offset;
    int length;
};

struct BSPHeader
{
    char        fourCC[4];
    int         version;
    BSPDirEntry dirEntries[17];
};

struct BSPTexture
{
    char    name[64];
    int     flags;
    int     contents;
};

struct BSPModel
{
    float   mins[3];
    float   maxs[3];
    int     face;
    int     n_faces;
    int     brush;
    int     n_brushes;
};

struct BSPVertex
{
    float           position[3];
    float           texcoord[2][2];
    float           normal[3];
    unsigned char   color[4];
};

struct BSPMeshvert
{
    int offset;
};

struct BSPFace
{
    int     texture;
    int     effect;
    int     type;
    int     vertex;
    int     n_vertices;
    int     meshvert;
    int     n_meshverts;
    int     lm_index;
    int     lm_start[2];
    int     lm_size[2];
    float   lm_origin[3];
    float   lm_vecs[2][3];
    float   normal[3];
    int     size[2];
};

char * data;

bool LoadBSP( const char * filename )
{
    FILE *fp;

    if (fopen_s(&fp,filename, "rb"))
        return false;

    fseek(fp,0,SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    data = reinterpret_cast< char * >( malloc(sizeof(char)*size) );

    if (fread(data,1,size,fp) != size)
        return false;

    fclose(fp);

    return true;
}

bool BuildCollada( const char * filename )
{
    const BSPHeader * header = reinterpret_cast< BSPHeader * >( data );

    const BSPTexture * textures = reinterpret_cast< BSPTexture * >( data + header->dirEntries[ LTextures ].offset );
    size_t textureCount = header->dirEntries[ LTextures ].length / sizeof(BSPTexture);

    const BSPModel * models = reinterpret_cast< BSPModel * >( data + header->dirEntries[ LModels ].offset );
    size_t modelCount = header->dirEntries[ LModels ].length / sizeof(BSPModel);

    const BSPVertex * vertices = reinterpret_cast< BSPVertex* >( data + header->dirEntries[ LVertices ].offset );
    size_t vertexCount = header->dirEntries[ LVertices ].length / sizeof(BSPVertex);

    const BSPMeshvert * meshverts = reinterpret_cast< BSPMeshvert * >( data + header->dirEntries[ LMeshverts ].offset );
    size_t meshvertCount = header->dirEntries[ LMeshverts ].length / sizeof(BSPMeshvert);

    const BSPFace * faces = reinterpret_cast< BSPFace * >( data + header->dirEntries[ LFaces ].offset );
    size_t faceCount = header->dirEntries[ LFaces ].length / sizeof(BSPFace);

    xmlDocPtr doc;

    doc = xmlNewDoc(BAD_CAST "1.0");

    // root
    xmlNodePtr root = xmlNewNode( NULL, BAD_CAST "COLLADA" );
    xmlNewProp( root, BAD_CAST "xmlns", BAD_CAST "http://www.collada.org/2005/11/COLLADASchema" );
    xmlNewProp( root, BAD_CAST "version", BAD_CAST "1.4.1" );
    xmlDocSetRootElement( doc, root );

    // library_images
    {
        xmlNodePtr lib_images = xmlNewChild( root, NULL, BAD_CAST "library_images", NULL );

        const BSPTexture * texture = textures;
        const BSPTexture * texture_end = textures + textureCount;
        for ( ; texture != texture_end; ++texture )
        {
            char strID[64];
            MakeStringID( texture->name, strID );

            xmlNodePtr image = xmlNewChild( lib_images, NULL, BAD_CAST "image", NULL );
            xmlNewProp( image, BAD_CAST "id", BAD_CAST strID );
            xmlNewProp( image, BAD_CAST "name", BAD_CAST strID );
            xmlNewChild( image, NULL, BAD_CAST "init_from", BAD_CAST texture->name );
        }
    }

    // library_effects
    {
        xmlNodePtr lib_effects = xmlNewChild( root, NULL, BAD_CAST "library_effects", NULL );

        const BSPTexture * texture = textures;
        const BSPTexture * texture_end = textures + textureCount;
        for ( ; texture != texture_end; ++texture )
        {
            char strID[64];
            MakeStringID( texture->name, strID );

            std::string effectID = strID;
            effectID += "-effect";

            xmlNodePtr effect = xmlNewChild( lib_effects, NULL, BAD_CAST "effect", NULL );
            xmlNewProp( effect, BAD_CAST "id", BAD_CAST effectID.c_str() );

            xmlNodePtr profile_common = xmlNewChild( effect, NULL, BAD_CAST "profile_COMMON", NULL );

            // surface
            std::string surfaceID = strID;
            surfaceID += "-surface";

            xmlNodePtr param_surface = xmlNewChild( profile_common, NULL, BAD_CAST "newparam", NULL );
            xmlNewProp( param_surface, BAD_CAST "sid", BAD_CAST surfaceID.c_str() );
            xmlNodePtr surface = xmlNewChild( param_surface, NULL, BAD_CAST "surface", NULL );
            xmlNewProp( surface, BAD_CAST "type", BAD_CAST "2D" );
            xmlNewChild( surface, NULL, BAD_CAST "init_from", BAD_CAST strID );

            // sampler
            std::string samplerID = strID;
            samplerID += "-sampler";

            xmlNodePtr param_sampler = xmlNewChild( profile_common, NULL, BAD_CAST "newparam", NULL );
            xmlNewProp( param_sampler, BAD_CAST "sid", BAD_CAST samplerID.c_str() );
            xmlNodePtr sampler = xmlNewChild( param_sampler, NULL, BAD_CAST "sampler2D", NULL );
            xmlNewChild( sampler, NULL, BAD_CAST "source", BAD_CAST surfaceID.c_str() );

            // technique
            xmlNodePtr technique = xmlNewChild( profile_common, NULL, BAD_CAST "technique", NULL );
            xmlNewProp( technique, BAD_CAST "sid", BAD_CAST "common" );
            xmlNodePtr phong = xmlNewChild( technique, NULL, BAD_CAST "phong", NULL );
            xmlNodePtr emission = xmlNewChild( phong, NULL, BAD_CAST "emission", NULL );
            xmlNodePtr emission_color = xmlNewChild( emission, NULL, BAD_CAST "color", BAD_CAST "0 0 0 1" );
            xmlNewProp( emission_color, BAD_CAST "sid", BAD_CAST "emission" );
            xmlNodePtr ambient = xmlNewChild( phong, NULL, BAD_CAST "ambient", NULL );
            xmlNodePtr ambient_color = xmlNewChild( ambient, NULL, BAD_CAST "color", BAD_CAST "0 0 0 1" );
            xmlNewProp( ambient_color, BAD_CAST "sid", BAD_CAST "ambient" );
            xmlNodePtr diffuse = xmlNewChild( phong, NULL, BAD_CAST "diffuse", NULL );
            xmlNodePtr diffuse_color = xmlNewChild( diffuse, NULL, BAD_CAST "color", BAD_CAST "0.5 0.5 0.5 1" );
            xmlNewProp( diffuse_color, BAD_CAST "sid", BAD_CAST "diffuse" );
            xmlNodePtr diffuse_texture = xmlNewChild( diffuse, NULL, BAD_CAST "texture", NULL );
            xmlNewProp( diffuse_texture, BAD_CAST "texture", BAD_CAST samplerID.c_str() );
            xmlNewProp( diffuse_texture, BAD_CAST "texcoord", BAD_CAST "UVMap" );
            xmlNodePtr specular = xmlNewChild( phong, NULL, BAD_CAST "specular", NULL );
            xmlNodePtr specular_color = xmlNewChild( specular, NULL, BAD_CAST "color", BAD_CAST "0.5 0.5 0.5 1" );
            xmlNewProp( specular_color, BAD_CAST "sid", BAD_CAST "specular" );
            xmlNodePtr shininess = xmlNewChild( phong, NULL, BAD_CAST "shininess", NULL );
            xmlNodePtr shininess_value = xmlNewChild( shininess, NULL, BAD_CAST "float", BAD_CAST "50" );
            xmlNewProp( shininess_value, BAD_CAST "sid", BAD_CAST "shininess" );
            xmlNodePtr index_of_refraction = xmlNewChild( phong, NULL, BAD_CAST "index_of_refraction", NULL );
            xmlNodePtr index_of_refraction_value = xmlNewChild( index_of_refraction, NULL, BAD_CAST "float", BAD_CAST "1" );
            xmlNewProp( index_of_refraction_value, BAD_CAST "sid", BAD_CAST "index_of_refraction" );
        }
    }

    // library_materials
    {
        xmlNodePtr lib_materials = xmlNewChild( root, NULL, BAD_CAST "library_materials", NULL );

        const BSPTexture * texture = textures;
        const BSPTexture * texture_end = textures + textureCount;
        for ( ; texture != texture_end; ++texture )
        {
            char strID[64];
            MakeStringID( texture->name, strID );

            std::string materialID = strID;
            materialID += "-material";

            std::string effectURL = "#";
            effectURL += strID;
            effectURL += "-effect";

            xmlNodePtr material = xmlNewChild( lib_materials, NULL, BAD_CAST "material", NULL );
            xmlNewProp( material, BAD_CAST "id", BAD_CAST materialID.c_str() );
            xmlNewProp( material, BAD_CAST "name", BAD_CAST strID );
            xmlNodePtr instance_effect = xmlNewChild( material, NULL, BAD_CAST "instance_effect", NULL );
            xmlNewProp( instance_effect, BAD_CAST "url", BAD_CAST effectURL.c_str() );
        }
    }

    // library_geometries
    {
        xmlNodePtr lib_geometries = xmlNewChild( root, NULL, BAD_CAST "library_geometries", NULL );

        size_t model_counter = 0;

        const BSPModel * model = models;
        const BSPModel * model_end = models + modelCount;
        for ( ; model != model_end; ++model, ++model_counter )
        {
            if ( model->n_faces == 0 )
                continue;

            char strID[64];
            sprintf( strID, "model_%i", model_counter );

            std::string geometryID = strID;
            geometryID += "-mesh";

            xmlNodePtr geometry = xmlNewChild( lib_geometries, NULL, BAD_CAST "geometry", NULL );
            xmlNewProp( geometry, BAD_CAST "id", BAD_CAST geometryID.c_str() );
            xmlNewProp( geometry, BAD_CAST "name", BAD_CAST strID );
            xmlNodePtr mesh = xmlNewChild( geometry, NULL, BAD_CAST "mesh", NULL );

            // positions
            {
                std::string positionsID = geometryID;
                positionsID += "-positions";
                
                xmlNodePtr source = xmlNewChild( mesh, NULL, BAD_CAST "source", NULL );
                xmlNewProp( source, BAD_CAST "id", BAD_CAST positionsID.c_str() );

                std::string positions_array;
                for ( size_t i=0; i<vertexCount; ++i )
                {
                    char pos[64];
                    sprintf( pos, "%f %f %f ", vertices[i].position[0], vertices[i].position[1], vertices[i].position[2] );

                    positions_array += pos;
                }

                std::string arrayID = positionsID;
                arrayID += "-array";

                size_t pos_count = vertexCount * 3;
                char ver_count_str[16];
                char pos_count_str[16];
                sprintf( ver_count_str, "%i", vertexCount );
                sprintf( pos_count_str, "%i", pos_count );

                xmlNodePtr float_array = xmlNewChild( source, NULL, BAD_CAST "float_array", BAD_CAST positions_array.c_str() );
                xmlNewProp( float_array, BAD_CAST "id", BAD_CAST arrayID.c_str() );
                xmlNewProp( float_array, BAD_CAST "count", BAD_CAST pos_count_str );

                std::string arrayIDRef = "#";
                arrayIDRef += arrayID;

                xmlNodePtr technique_common = xmlNewChild( source, NULL, BAD_CAST "technique_common", NULL );
                xmlNodePtr accessor = xmlNewChild( technique_common, NULL, BAD_CAST "accessor", NULL );
                xmlNewProp( accessor, BAD_CAST "source", BAD_CAST arrayIDRef.c_str() );
                xmlNewProp( accessor, BAD_CAST "count", BAD_CAST ver_count_str );
                xmlNewProp( accessor, BAD_CAST "stride", BAD_CAST "3" );

                xmlNodePtr paramX = xmlNewChild( accessor, NULL, BAD_CAST "param", NULL );
                xmlNewProp( paramX, BAD_CAST "name", BAD_CAST "X" );
                xmlNewProp( paramX, BAD_CAST "type", BAD_CAST "float" );

                xmlNodePtr paramY = xmlNewChild( accessor, NULL, BAD_CAST "param", NULL );
                xmlNewProp( paramY, BAD_CAST "name", BAD_CAST "Y" );
                xmlNewProp( paramY, BAD_CAST "type", BAD_CAST "float" );

                xmlNodePtr paramZ = xmlNewChild( accessor, NULL, BAD_CAST "param", NULL );
                xmlNewProp( paramZ, BAD_CAST "name", BAD_CAST "Z" );
                xmlNewProp( paramZ, BAD_CAST "type", BAD_CAST "float" );
            }

            // normals
            {
                std::string normalsID = geometryID;
                normalsID += "-normals";
                
                xmlNodePtr source = xmlNewChild( mesh, NULL, BAD_CAST "source", NULL );
                xmlNewProp( source, BAD_CAST "id", BAD_CAST normalsID.c_str() );

                std::string normals_array;
                for ( size_t i=0; i<vertexCount; ++i )
                {
                    char pos[64];
                    sprintf( pos, "%f %f %f ", vertices[i].normal[0], vertices[i].normal[1], vertices[i].normal[2] );

                    normals_array += pos;
                }

                std::string arrayID = normalsID;
                arrayID += "-array";

                size_t nor_count = vertexCount * 3;
                char ver_count_str[16];
                char nor_count_str[16];
                sprintf( ver_count_str, "%i", vertexCount );
                sprintf( nor_count_str, "%i", nor_count );

                xmlNodePtr float_array = xmlNewChild( source, NULL, BAD_CAST "float_array", BAD_CAST normals_array.c_str() );
                xmlNewProp( float_array, BAD_CAST "id", BAD_CAST arrayID.c_str() );
                xmlNewProp( float_array, BAD_CAST "count", BAD_CAST nor_count_str );

                std::string arrayIDRef = "#";
                arrayIDRef += arrayID;

                xmlNodePtr technique_common = xmlNewChild( source, NULL, BAD_CAST "technique_common", NULL );
                xmlNodePtr accessor = xmlNewChild( technique_common, NULL, BAD_CAST "accessor", NULL );
                xmlNewProp( accessor, BAD_CAST "source", BAD_CAST arrayIDRef.c_str() );
                xmlNewProp( accessor, BAD_CAST "count", BAD_CAST ver_count_str );
                xmlNewProp( accessor, BAD_CAST "stride", BAD_CAST "3" );

                xmlNodePtr paramX = xmlNewChild( accessor, NULL, BAD_CAST "param", NULL );
                xmlNewProp( paramX, BAD_CAST "name", BAD_CAST "X" );
                xmlNewProp( paramX, BAD_CAST "type", BAD_CAST "float" );

                xmlNodePtr paramY = xmlNewChild( accessor, NULL, BAD_CAST "param", NULL );
                xmlNewProp( paramY, BAD_CAST "name", BAD_CAST "Y" );
                xmlNewProp( paramY, BAD_CAST "type", BAD_CAST "float" );

                xmlNodePtr paramZ = xmlNewChild( accessor, NULL, BAD_CAST "param", NULL );
                xmlNewProp( paramZ, BAD_CAST "name", BAD_CAST "Z" );
                xmlNewProp( paramZ, BAD_CAST "type", BAD_CAST "float" );
            }

            // texcoords
            {
                std::string texcoordsID = geometryID;
                texcoordsID += "-map-0";
                
                xmlNodePtr source = xmlNewChild( mesh, NULL, BAD_CAST "source", NULL );
                xmlNewProp( source, BAD_CAST "id", BAD_CAST texcoordsID.c_str() );

                std::string texcoords_array;
                for ( size_t i=0; i<vertexCount; ++i )
                {
                    char pos[64];
                    sprintf( pos, "%f %f ", vertices[i].texcoord[0][0], vertices[i].texcoord[0][1] );

                    texcoords_array += pos;
                }

                std::string arrayID = texcoordsID;
                arrayID += "-array";

                size_t uvs_count = vertexCount * 2;
                char ver_count_str[16];
                char uvs_count_str[16];
                sprintf( ver_count_str, "%i", vertexCount );
                sprintf( uvs_count_str, "%i", uvs_count );

                xmlNodePtr float_array = xmlNewChild( source, NULL, BAD_CAST "float_array", BAD_CAST texcoords_array.c_str() );
                xmlNewProp( float_array, BAD_CAST "id", BAD_CAST arrayID.c_str() );
                xmlNewProp( float_array, BAD_CAST "count", BAD_CAST uvs_count_str );

                std::string arrayIDRef = "#";
                arrayIDRef += arrayID;

                xmlNodePtr technique_common = xmlNewChild( source, NULL, BAD_CAST "technique_common", NULL );
                xmlNodePtr accessor = xmlNewChild( technique_common, NULL, BAD_CAST "accessor", NULL );
                xmlNewProp( accessor, BAD_CAST "source", BAD_CAST arrayIDRef.c_str() );
                xmlNewProp( accessor, BAD_CAST "count", BAD_CAST ver_count_str );
                xmlNewProp( accessor, BAD_CAST "stride", BAD_CAST "2" );

                xmlNodePtr paramS = xmlNewChild( accessor, NULL, BAD_CAST "param", NULL );
                xmlNewProp( paramS, BAD_CAST "name", BAD_CAST "S" );
                xmlNewProp( paramS, BAD_CAST "type", BAD_CAST "float" );

                xmlNodePtr paramT = xmlNewChild( accessor, NULL, BAD_CAST "param", NULL );
                xmlNewProp( paramT, BAD_CAST "name", BAD_CAST "T" );
                xmlNewProp( paramT, BAD_CAST "type", BAD_CAST "float" );
            }

            std::string verticesID = geometryID;
            verticesID += "-vertices";
                
            xmlNodePtr vertices_node = xmlNewChild( mesh, NULL, BAD_CAST "vertices", NULL );
            xmlNewProp( vertices_node, BAD_CAST "id", BAD_CAST verticesID.c_str() );

            std::string positionsIDRef = "#";
            positionsIDRef += geometryID;
            positionsIDRef += "-positions";

            xmlNodePtr input_node = xmlNewChild( vertices_node, NULL, BAD_CAST "input", NULL );
            xmlNewProp( input_node, BAD_CAST "semantic", BAD_CAST "POSITION" );
            xmlNewProp( input_node, BAD_CAST "source", BAD_CAST positionsIDRef.c_str() );

            // polylists
            {
                std::vector< int > index_buffer;

                const BSPFace * face = faces + model->face;
                const BSPFace * face_end = faces + model->face + model->n_faces;
                for ( ; face != face_end; ++face )
                {
                    if ( face->type == 1 || face->type == 3 )
                    {
                        const BSPMeshvert * meshvert = meshverts + face->meshvert;
                        const BSPMeshvert * meshvert_end = meshverts + face->meshvert + face->n_meshverts;
                        for ( ; meshvert != meshvert_end; ++meshvert )
                        {
                            index_buffer.push_back( face->vertex + meshvert->offset );
                        }
                    }

                    if (    face->type == 2
                        ||  face->type == 4
                        ||  ( face + 1 == face_end )
                        ||  face->type != (face+1)->type
                        ||  face->texture != (face+1)->texture )
                    {
                        if ( ! index_buffer.empty() )
                        {
                            const BSPTexture * texture = textures + face->texture;

                            char strID[64];
                            MakeStringID( texture->name, strID );

                            std::string materialIDRef = "#";
                            materialIDRef += strID;
                            materialIDRef += "-material";

                            size_t poly_count = index_buffer.size() / 3;
                            char poly_count_str[16];
                            sprintf( poly_count_str, "%i", poly_count );

                            xmlNodePtr polylist = xmlNewChild( mesh, NULL, BAD_CAST "polylist", NULL );
                            xmlNewProp( polylist, BAD_CAST "material", BAD_CAST materialIDRef.c_str() );
                            xmlNewProp( polylist, BAD_CAST "count", BAD_CAST poly_count_str );

                            std::string verticesIDRef = "#";
                            verticesIDRef += geometryID;
                            verticesIDRef += "-vertices";

                            xmlNodePtr input_vertices = xmlNewChild( polylist, NULL, BAD_CAST "input", NULL );
                            xmlNewProp( input_vertices, BAD_CAST "semantic", BAD_CAST "VERTEX" );
                            xmlNewProp( input_vertices, BAD_CAST "source", BAD_CAST verticesIDRef.c_str() );
                            xmlNewProp( input_vertices, BAD_CAST "offset", BAD_CAST "0" );

                            std::string normalsIDRef = "#";
                            normalsIDRef += geometryID;
                            normalsIDRef += "-normals";

                            xmlNodePtr input_normals = xmlNewChild( polylist, NULL, BAD_CAST "input", NULL );
                            xmlNewProp( input_normals, BAD_CAST "semantic", BAD_CAST "NORMAL" );
                            xmlNewProp( input_normals, BAD_CAST "source", BAD_CAST normalsIDRef.c_str() );
                            xmlNewProp( input_normals, BAD_CAST "offset", BAD_CAST "1" );

                            std::string texcoordsIDRef = "#";
                            texcoordsIDRef += geometryID;
                            texcoordsIDRef += "-map-0";

                            xmlNodePtr input_texcoords = xmlNewChild( polylist, NULL, BAD_CAST "input", NULL );
                            xmlNewProp( input_texcoords, BAD_CAST "semantic", BAD_CAST "TEXCOORD" );
                            xmlNewProp( input_texcoords, BAD_CAST "source", BAD_CAST texcoordsIDRef.c_str() );
                            xmlNewProp( input_texcoords, BAD_CAST "offset", BAD_CAST "2" );
                            xmlNewProp( input_texcoords, BAD_CAST "set", BAD_CAST "0" );

                            std::string vcount;
                            std::string p;

                            for ( size_t i=0; i<poly_count; ++i )
                            {
                                vcount += "3 ";
                                
                                size_t offset = 3 * i;
                                char triangle_str[128];
                                sprintf( triangle_str, "%i %i %i %i %i %i %i %i %i ", index_buffer[offset+0], index_buffer[offset+0], index_buffer[offset+0]
                                                                                    , index_buffer[offset+1], index_buffer[offset+1], index_buffer[offset+1]
                                                                                    , index_buffer[offset+2], index_buffer[offset+2], index_buffer[offset+2] );

                                p += triangle_str;
                            }

                            xmlNewChild( polylist, NULL, BAD_CAST "vcount", BAD_CAST vcount.c_str() );
                            xmlNewChild( polylist, NULL, BAD_CAST "p", BAD_CAST p.c_str() );

                            index_buffer.clear();
                        }
                    }
                }
            }
        }
    }

    // library_visual_scenes
    {
        xmlNodePtr lib_visual_scenes = xmlNewChild( root, NULL, BAD_CAST "library_visual_scenes", NULL );
        xmlNodePtr visual_scene = xmlNewChild( lib_visual_scenes, NULL, BAD_CAST "visual_scene", NULL );
        xmlNewProp( visual_scene, BAD_CAST "id", BAD_CAST "Scene" );
        xmlNewProp( visual_scene, BAD_CAST "name", BAD_CAST "Scene" );

        size_t model_counter = 0;

        const BSPModel * model = models;
        const BSPModel * model_end = models + modelCount;
        for ( ; model != model_end; ++model, ++model_counter )
        {
            if ( model->n_faces == 0 )
                continue;

            char strID[64];
            sprintf( strID, "model_%i", model_counter );

            xmlNodePtr node = xmlNewChild( visual_scene, NULL, BAD_CAST "node", NULL );
            xmlNewProp( node, BAD_CAST "id", BAD_CAST strID );
            xmlNewProp( node, BAD_CAST "name", BAD_CAST strID );
            xmlNewProp( node, BAD_CAST "type", BAD_CAST "NODE" );

            xmlNodePtr translate = xmlNewChild( node, NULL, BAD_CAST "translate", BAD_CAST "0 0 0" );
            xmlNewProp( translate, BAD_CAST "sid", BAD_CAST "location" );

            xmlNodePtr rotateZ = xmlNewChild( node, NULL, BAD_CAST "rotate", BAD_CAST "0 0 1 0" );
            xmlNewProp( rotateZ, BAD_CAST "sid", BAD_CAST "rotationZ" );

            xmlNodePtr rotateY = xmlNewChild( node, NULL, BAD_CAST "rotate", BAD_CAST "0 1 0 0" );
            xmlNewProp( rotateY, BAD_CAST "sid", BAD_CAST "rotationY" );

            xmlNodePtr rotateX = xmlNewChild( node, NULL, BAD_CAST "rotate", BAD_CAST "1 0 0 0" );
            xmlNewProp( rotateX, BAD_CAST "sid", BAD_CAST "rotationX" );

            xmlNodePtr scale = xmlNewChild( node, NULL, BAD_CAST "scale", BAD_CAST "1 1 1" );
            xmlNewProp( scale, BAD_CAST "sid", BAD_CAST "scale" );

            std::string geometryIDRef = "#";
            geometryIDRef += strID;
            geometryIDRef += "-mesh";

            xmlNodePtr instance_geometry = xmlNewChild( node, NULL, BAD_CAST "instance_geometry", NULL );
            xmlNewProp( instance_geometry, BAD_CAST "url", BAD_CAST geometryIDRef.c_str() );

            xmlNodePtr bind_material = xmlNewChild( instance_geometry, NULL, BAD_CAST "bind_material", NULL );
            xmlNodePtr technique_common = xmlNewChild( bind_material, NULL, BAD_CAST "technique_common", NULL );

            const BSPTexture * texture = textures;
            const BSPTexture * texture_end = textures + textureCount;
            for ( ; texture != texture_end; ++texture )
            {
                char strID[64];
                MakeStringID( texture->name, strID );

                std::string materialID = strID;
                materialID += "-material";

                std::string materialIDRef = "#";
                materialIDRef += materialID;

                xmlNodePtr instance_material = xmlNewChild( technique_common, NULL, BAD_CAST "instance_material", NULL );
                xmlNewProp( instance_material, BAD_CAST "symbol", BAD_CAST materialID.c_str() );
                xmlNewProp( instance_material, BAD_CAST "target", BAD_CAST materialIDRef.c_str() );

                xmlNodePtr bind_vertex_input = xmlNewChild( instance_material, NULL, BAD_CAST "bind_vertex_input", NULL );
                xmlNewProp( bind_vertex_input, BAD_CAST "semantic", BAD_CAST "UVMap" );
                xmlNewProp( bind_vertex_input, BAD_CAST "input_semantic", BAD_CAST "TEXCOORD" );
                xmlNewProp( bind_vertex_input, BAD_CAST "input_set", BAD_CAST "0" );
            }
        }
    }

    // scene
    {
        xmlNodePtr scene = xmlNewChild( root, NULL, BAD_CAST "scene", NULL );
        xmlNodePtr instance_visual_scene = xmlNewChild( scene, NULL, BAD_CAST "instance_visual_scene", NULL );
        xmlNewProp( instance_visual_scene, BAD_CAST "url", BAD_CAST "#Scene" );
    }

    xmlChar *xmlbuff;
    int buffersize;

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

    FILE *fp;

    if (fopen_s(&fp,filename, "wb"))
    {
        xmlFree(xmlbuff);
        xmlFreeDoc(doc);
        return false;
    }

    if (fwrite(xmlbuff,1,buffersize,fp) != buffersize)
    {
        fclose(fp);
        xmlFree(xmlbuff);
        xmlFreeDoc(doc);
        return false;
    }

    fclose(fp);

    xmlFree(xmlbuff);
    xmlFreeDoc(doc);

    return true;
}

bool ConvertBSP( const char * inFilename, const char * outFilename )
{
    if ( ! LoadBSP( inFilename ) )
        return false;

    if ( ! BuildCollada( outFilename ) )
    {
        free(data);
        return false;
    }

    free(data);

    return true;
}
