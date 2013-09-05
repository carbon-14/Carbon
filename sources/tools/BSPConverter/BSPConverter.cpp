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

bool RootPath( const char * ifilename, char * path )
{
#if defined( CARBON_PLATFORM_WIN32 )
    char dir[ 256 ];
    char * fileStart;
    if ( GetFullPathName( ifilename, 256, dir, &fileStart ) == 0 )
    {
        return false;
    }

    strcpy( fileStart, ".." );

    if ( GetFullPathName( dir, 256, path, &fileStart ) == 0 )
    {
        return false;
    }

#endif
}

bool FindFileExtension( const char * ifilename, char * extension )
{
#if defined( CARBON_PLATFORM_WIN32 )
    WIN32_FIND_DATA find_data;
    HANDLE handle;
    handle = FindFirstFile( ifilename, &find_data );
    if ( handle == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    bool fmt = false;
    char * ptr = find_data.cFileName;
    while ( *ptr != 0 )
    {
        if ( fmt )
        {
            *(extension++) = *ptr;
        }
        else if ( *ptr == '.' )
        {
            fmt = true;
        }

        ++ptr;
    }

    *extension = 0;

    FindClose(handle);
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

float QuadraticBezierInterpolation( float i, float c0, float c1, float c2 )
{
    float invi = 1.0f - i;
    return c0 * invi * invi  +  c1 * 2.0f * i * invi  +  c2 * i * i;
}

float QuadraticBezierSurface( float u, float v, const float * ctrl )
{
    float a = QuadraticBezierInterpolation( u, ctrl[0], ctrl[1], ctrl[2] );
    float b = QuadraticBezierInterpolation( u, ctrl[3], ctrl[4], ctrl[5] );
    float c = QuadraticBezierInterpolation( u, ctrl[6], ctrl[7], ctrl[8] );

    return QuadraticBezierInterpolation( v, a, b, c );
}

float DerivateQuadraticBezierInterpolation( float i, float c0, float c1, float c2 )
{
    return 2.0f * ( c0 * ( i - 1.0f )  +  c1 * ( 1.0f - 2.0f * i )  +  c2 * i );
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

struct COLLADAImage
{
    std::string id;
    std::string name;
    std::string init_from;
};

struct COLLADAEffect
{
    std::string id;
    std::string surface_id;
    std::string surface_init_from;
    std::string sampler_id;
};

struct COLLADAMaterial
{
    std::string id;
    std::string name;
    std::string instance_effect;
};

struct COLLADAGeometrySource
{
    std::string             id;
    std::string             float_array_id;
    std::vector< float >    float_array;
};

struct COLLADAGeometryPolylist
{
    std::string         material;
    std::vector< int >  p;
};

struct COLLADAGeometry
{
    std::string                             id;
    std::string                             name;
    COLLADAGeometrySource                   positions;
    COLLADAGeometrySource                   normals;
    COLLADAGeometrySource                   texcoords;
    std::string                             vertices_id;
    std::vector< COLLADAGeometryPolylist >  polylists;
};

char root_path[128];

size_t tesselation_steps = 1;

char * data;

std::vector< COLLADAImage >     library_images;
std::vector< COLLADAEffect >    library_effects;
std::vector< COLLADAMaterial >  library_materials;
std::vector< COLLADAGeometry >  library_geometries;

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

    // library_images
    {
        const BSPTexture * texture = textures;
        const BSPTexture * texture_end = textures + textureCount;
        for ( ; texture != texture_end; ++texture )
        {
            COLLADAImage image;

            char strID[64];
            MakeStringID( texture->name, strID );

            image.id        = strID;
            image.name      = strID;
            image.init_from = texture->name;

            char texture_name[128];
            sprintf( texture_name, "%s/%s.*", root_path, texture->name );

            char file_extension[8];

            if ( FindFileExtension( texture_name, file_extension ) )
            {
                image.init_from += ".";
                image.init_from += file_extension;
            }

            library_images.push_back( image );
        }
    }

    // library_effects
    {
        const BSPTexture * texture = textures;
        const BSPTexture * texture_end = textures + textureCount;
        for ( ; texture != texture_end; ++texture )
        {
            COLLADAEffect effect;

            char strID[64];
            MakeStringID( texture->name, strID );

            effect.id                   = strID;
            effect.id                   += "-effect";
            effect.surface_id           = strID;
            effect.surface_id           += "-surface";
            effect.surface_init_from    = strID;
            effect.sampler_id           = strID;
            effect.sampler_id           += "-sampler";

            library_effects.push_back( effect );
        }
    }

    // library_materials
    {
        const BSPTexture * texture = textures;
        const BSPTexture * texture_end = textures + textureCount;
        for ( ; texture != texture_end; ++texture )
        {
            COLLADAMaterial material;

            char strID[64];
            MakeStringID( texture->name, strID );

            material.id                 = strID;
            material.id                 += "-material";
            material.name               = strID;
            material.instance_effect    = "#";
            material.instance_effect    += strID;
            material.instance_effect    += "-effect";

            library_materials.push_back( material );
        }
    }

    // library_geometries
    {
        size_t model_counter = 0;

        const BSPModel * model = models;
        const BSPModel * model_end = models + modelCount;
        for ( ; model != model_end; ++model, ++model_counter )
        {
            if ( model->n_faces == 0 )
                continue;

            COLLADAGeometry geometry;

            char strID[64];
            sprintf( strID, "model_%i", model_counter );

            geometry.id = strID;
            geometry.id += "-mesh";
            geometry.name   = strID;

            // positions
            {
                COLLADAGeometrySource& positions = geometry.positions;

                positions.id = geometry.id;
                positions.id += "-positions";

                positions.float_array_id = positions.id;
                positions.float_array_id += "-array";

                for ( size_t i=0; i<vertexCount; ++i )
                {
                    positions.float_array.push_back( vertices[i].position[0] );
                    positions.float_array.push_back( vertices[i].position[1] );
                    positions.float_array.push_back( vertices[i].position[2] );
                }
            }

            // normals
            {
                COLLADAGeometrySource& normals = geometry.normals;

                normals.id = geometry.id;
                normals.id += "-normals";

                normals.float_array_id = normals.id;
                normals.float_array_id += "-array";

                for ( size_t i=0; i<vertexCount; ++i )
                {
                    normals.float_array.push_back( vertices[i].normal[0] );
                    normals.float_array.push_back( vertices[i].normal[1] );
                    normals.float_array.push_back( vertices[i].normal[2] );
                }
            }

            // texcoords
            {
                COLLADAGeometrySource& texcoords = geometry.texcoords;

                texcoords.id = geometry.id;
                texcoords.id += "-map-0";

                texcoords.float_array_id = texcoords.id;
                texcoords.float_array_id += "-array";

                for ( size_t i=0; i<vertexCount; ++i )
                {
                    texcoords.float_array.push_back( vertices[i].texcoord[0][0] );
                    texcoords.float_array.push_back( 1.0f - vertices[i].texcoord[0][1] );
                }
            }

            geometry.vertices_id = geometry.id;
            geometry.vertices_id += "-vertices";

            // polylists
            {
                std::vector< int > index_buffer;

                const BSPFace * face = faces + model->face;
                const BSPFace * face_end = faces + model->face + model->n_faces;
                for ( ; face != face_end; ++face )
                {
                    if ( face->type == 1 || face->type == 2 || face->type == 3 )
                    {
                        // Bezier patches
                        if ( face->type == 2 )
                        {
                            for ( size_t i=0; i<(face->size[0]-1)/2; ++i )
                            {
                                for ( size_t j=0; j<(face->size[1]-1)/2; ++j )
                                {
                                    int ctrl_points[9];
                                    for ( size_t u=0; u<3; ++u )
                                    {
                                        for ( size_t v=0; v<3; ++v )
                                        {
                                            ctrl_points[ u + 3 * v ] = face->vertex + 2 * i + u + ( 2 * j + v ) * face->size[0];
                                        }
                                    }

                                    size_t x_tesselation_steps = 1;
                                    for ( size_t k=0; k<3; ++k )
                                    {
                                        float v0[3];
                                        float v1[3];

                                        v0[0] = vertices[ ctrl_points[0+3*k] ].position[0] - vertices[ ctrl_points[1+3*k] ].position[0];
                                        v0[1] = vertices[ ctrl_points[0+3*k] ].position[1] - vertices[ ctrl_points[1+3*k] ].position[1];
                                        v0[2] = vertices[ ctrl_points[0+3*k] ].position[2] - vertices[ ctrl_points[1+3*k] ].position[2];
                                        float v0_len = sqrt( v0[0]*v0[0] + v0[1]*v0[1] + v0[2]*v0[2] );
                                        v0[0] /= v0_len;
                                        v0[1] /= v0_len;
                                        v0[2] /= v0_len;

                                        v1[0] = vertices[ ctrl_points[2+3*k] ].position[0] - vertices[ ctrl_points[1+3*k] ].position[0];
                                        v1[1] = vertices[ ctrl_points[2+3*k] ].position[1] - vertices[ ctrl_points[1+3*k] ].position[1];
                                        v1[2] = vertices[ ctrl_points[2+3*k] ].position[2] - vertices[ ctrl_points[1+3*k] ].position[2];
                                        float v1_len = sqrt( v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2] );
                                        v1[0] /= v1_len;
                                        v1[1] /= v1_len;
                                        v1[2] /= v1_len;

                                        if ( ( v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2] ) > -0.9f )
                                        {
                                            x_tesselation_steps = tesselation_steps;
                                            break;
                                        }
                                    }

                                    size_t y_tesselation_steps = 1;
                                    for ( size_t k=0; k<3; ++k )
                                    {
                                        float v0[3];
                                        float v1[3];

                                        v0[0] = vertices[ ctrl_points[0+k] ].position[0] - vertices[ ctrl_points[3+k] ].position[0];
                                        v0[1] = vertices[ ctrl_points[0+k] ].position[1] - vertices[ ctrl_points[3+k] ].position[1];
                                        v0[2] = vertices[ ctrl_points[0+k] ].position[2] - vertices[ ctrl_points[3+k] ].position[2];
                                        float v0_len = sqrt( v0[0]*v0[0] + v0[1]*v0[1] + v0[2]*v0[2] );
                                        v0[0] /= v0_len;
                                        v0[1] /= v0_len;
                                        v0[2] /= v0_len;

                                        v1[0] = vertices[ ctrl_points[6+k] ].position[0] - vertices[ ctrl_points[3+k] ].position[0];
                                        v1[1] = vertices[ ctrl_points[6+k] ].position[1] - vertices[ ctrl_points[3+k] ].position[1];
                                        v1[2] = vertices[ ctrl_points[6+k] ].position[2] - vertices[ ctrl_points[3+k] ].position[2];
                                        float v1_len = sqrt( v1[0]*v1[0] + v1[1]*v1[1] + v1[2]*v1[2] );
                                        v1[0] /= v1_len;
                                        v1[1] /= v1_len;
                                        v1[2] /= v1_len;

                                        if ( ( v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2] ) > -0.9f )
                                        {
                                            y_tesselation_steps = tesselation_steps;
                                            break;
                                        }
                                    }

                                    const size_t x_bsize = x_tesselation_steps + 1;
                                    const size_t y_bsize = y_tesselation_steps + 1;

                                    size_t offset = geometry.positions.float_array.size() / 3;

                                    for ( size_t v=0; v<y_bsize; ++v )
                                    {
                                        const float v_ratio = static_cast<float>(v) / static_cast<float>(y_tesselation_steps);

                                        for ( size_t u=0; u<x_bsize; ++u )
                                        {
                                            const float u_ratio = static_cast<float>(u) / static_cast<float>(x_tesselation_steps);

                                            float du[3];
                                            float dv[3];

                                            float p[9];
                                            {
                                                for ( size_t k=0; k<9; ++k )    { p[ k ] = vertices[ ctrl_points[ k ] ].position[0];    }
                                                geometry.positions.float_array.push_back( QuadraticBezierSurface( u_ratio, v_ratio, p ) );
                                            }
                                            {
                                                for ( size_t k=0; k<9; ++k )    { p[ k ] = vertices[ ctrl_points[ k ] ].position[1];    }
                                                geometry.positions.float_array.push_back( QuadraticBezierSurface( u_ratio, v_ratio, p ) );
                                            }
                                            {
                                                for ( size_t k=0; k<9; ++k )    { p[ k ] = vertices[ ctrl_points[ k ] ].position[2];    }
                                                geometry.positions.float_array.push_back( QuadraticBezierSurface( u_ratio, v_ratio, p ) );
                                            }
                                            {
                                                for ( size_t k=0; k<9; ++k )    { p[ k ] = vertices[ ctrl_points[ k ] ].texcoord[0][0]; }
                                                geometry.texcoords.float_array.push_back( QuadraticBezierSurface( u_ratio, v_ratio, p ) );
                                            }
                                            {
                                                for ( size_t k=0; k<9; ++k )    { p[ k ] = vertices[ ctrl_points[ k ] ].texcoord[0][1]; }
                                                geometry.texcoords.float_array.push_back( 1.0f - QuadraticBezierSurface( u_ratio, v_ratio, p ) );
                                            }

                                            geometry.normals.float_array.push_back( 0.0f );
                                            geometry.normals.float_array.push_back( 0.0f );
                                            geometry.normals.float_array.push_back( 1.0f );
                                        }
                                    }

                                    for ( size_t u=0; u<x_tesselation_steps; ++u )
                                    {
                                        for ( size_t v=0; v<y_tesselation_steps; ++v )
                                        {
                                            index_buffer.push_back( offset + u + v * x_bsize );
                                            index_buffer.push_back( offset + (u+1) + (v+1) * x_bsize );
                                            index_buffer.push_back( offset + u + (v+1) * x_bsize );
                                    
                                            index_buffer.push_back( offset + u + v * x_bsize );
                                            index_buffer.push_back( offset + (u+1) + v * x_bsize );
                                            index_buffer.push_back( offset + (u+1) + (v+1) * x_bsize );
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            const BSPMeshvert * meshvert = meshverts + face->meshvert;
                            const BSPMeshvert * meshvert_end = meshverts + face->meshvert + face->n_meshverts;
                            for ( ; meshvert != meshvert_end; meshvert += 3 )
                            {
                                index_buffer.push_back( face->vertex + meshvert->offset );
                                index_buffer.push_back( face->vertex + (meshvert+2)->offset );
                                index_buffer.push_back( face->vertex + (meshvert+1)->offset );
                            }
                        }
                    }

                    if (    face->type == 4
                        ||  (face+1) == face_end
                        ||  (face+1)->texture != face->texture
                        ||  (face+1)->type != face->type )
                    {
                        if ( ! index_buffer.empty() )
                        {
                            COLLADAGeometryPolylist polylist;

                            polylist.material = library_materials[ face->texture ].id;
                            polylist.p = index_buffer;

                            geometry.polylists.push_back( polylist );

                            index_buffer.clear();
                        }
                    }
                }
            }

            library_geometries.push_back( geometry );
        }
    }

    // write XML document

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

        std::vector< COLLADAImage >::const_iterator it = library_images.begin();
        std::vector< COLLADAImage >::const_iterator end = library_images.end();
        for ( ; it != end; ++it )
        {
            xmlNodePtr image = xmlNewChild( lib_images, NULL, BAD_CAST "image", NULL );
            xmlNewProp( image, BAD_CAST "id", BAD_CAST it->id.c_str() );
            xmlNewProp( image, BAD_CAST "name", BAD_CAST it->name.c_str() );
            xmlNewChild( image, NULL, BAD_CAST "init_from", BAD_CAST it->init_from.c_str() );
        }
    }

    // library_effects
    {
        xmlNodePtr lib_effects = xmlNewChild( root, NULL, BAD_CAST "library_effects", NULL );

        std::vector< COLLADAEffect >::const_iterator it = library_effects.begin();
        std::vector< COLLADAEffect >::const_iterator end = library_effects.end();
        for ( ; it != end; ++it )
        {

            xmlNodePtr effect = xmlNewChild( lib_effects, NULL, BAD_CAST "effect", NULL );
            xmlNewProp( effect, BAD_CAST "id", BAD_CAST it->id.c_str() );

            xmlNodePtr profile_common = xmlNewChild( effect, NULL, BAD_CAST "profile_COMMON", NULL );

            xmlNodePtr param_surface = xmlNewChild( profile_common, NULL, BAD_CAST "newparam", NULL );
            xmlNewProp( param_surface, BAD_CAST "sid", BAD_CAST it->surface_id.c_str() );
            xmlNodePtr surface = xmlNewChild( param_surface, NULL, BAD_CAST "surface", NULL );
            xmlNewProp( surface, BAD_CAST "type", BAD_CAST "2D" );
            xmlNewChild( surface, NULL, BAD_CAST "init_from", BAD_CAST it->surface_init_from.c_str() );

            xmlNodePtr param_sampler = xmlNewChild( profile_common, NULL, BAD_CAST "newparam", NULL );
            xmlNewProp( param_sampler, BAD_CAST "sid", BAD_CAST it->sampler_id.c_str() );
            xmlNodePtr sampler = xmlNewChild( param_sampler, NULL, BAD_CAST "sampler2D", NULL );
            xmlNewChild( sampler, NULL, BAD_CAST "source", BAD_CAST it->surface_id.c_str() );

            // technique
            xmlNodePtr technique = xmlNewChild( profile_common, NULL, BAD_CAST "technique", NULL );
            xmlNewProp( technique, BAD_CAST "sid", BAD_CAST "common" );
            xmlNodePtr phong = xmlNewChild( technique, NULL, BAD_CAST "phong", NULL );
            xmlNodePtr emission = xmlNewChild( phong, NULL, BAD_CAST "emission", NULL );
            xmlNodePtr emission_color = xmlNewChild( emission, NULL, BAD_CAST "color", BAD_CAST "0 0 0 1" );
            xmlNewProp( emission_color, BAD_CAST "sid", BAD_CAST "emission" );
            xmlNodePtr ambient = xmlNewChild( phong, NULL, BAD_CAST "ambient", NULL );
            xmlNodePtr ambient_texture = xmlNewChild( ambient, NULL, BAD_CAST "texture", NULL );
<<<<<<< HEAD
            xmlNewProp( ambient_texture, BAD_CAST "texture", BAD_CAST it->sampler_id.c_str() );
=======
            xmlNewProp( ambient_texture, BAD_CAST "texture", BAD_CAST samplerID.c_str() );
>>>>>>> 2082577e3a931bf6885ba1b3dcc2de8c9f0c638b
            xmlNewProp( ambient_texture, BAD_CAST "texcoord", BAD_CAST "UVMap" );
            xmlNodePtr diffuse = xmlNewChild( phong, NULL, BAD_CAST "diffuse", NULL );
            xmlNodePtr diffuse_texture = xmlNewChild( diffuse, NULL, BAD_CAST "texture", NULL );
            xmlNewProp( diffuse_texture, BAD_CAST "texture", BAD_CAST it->sampler_id.c_str() );
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

        std::vector< COLLADAMaterial >::const_iterator it = library_materials.begin();
        std::vector< COLLADAMaterial >::const_iterator end = library_materials.end();
        for ( ; it != end; ++it )
        {
            xmlNodePtr material = xmlNewChild( lib_materials, NULL, BAD_CAST "material", NULL );
            xmlNewProp( material, BAD_CAST "id", BAD_CAST it->id.c_str() );
            xmlNewProp( material, BAD_CAST "name", BAD_CAST it->name.c_str() );
            xmlNodePtr instance_effect = xmlNewChild( material, NULL, BAD_CAST "instance_effect", NULL );
            xmlNewProp( instance_effect, BAD_CAST "url", BAD_CAST it->instance_effect.c_str() );
        }
    }

    // library_geometries
    {
        xmlNodePtr lib_geometries = xmlNewChild( root, NULL, BAD_CAST "library_geometries", NULL );

        std::vector< COLLADAGeometry >::const_iterator it = library_geometries.begin();
        std::vector< COLLADAGeometry >::const_iterator end = library_geometries.end();
        for ( ; it != end; ++it )
        {
            xmlNodePtr geometry = xmlNewChild( lib_geometries, NULL, BAD_CAST "geometry", NULL );
            xmlNewProp( geometry, BAD_CAST "id", BAD_CAST it->id.c_str() );
            xmlNewProp( geometry, BAD_CAST "name", BAD_CAST it->name.c_str() );
            xmlNodePtr mesh = xmlNewChild( geometry, NULL, BAD_CAST "mesh", NULL );

            // positions
            {
                const COLLADAGeometrySource& positions = it->positions;
                
                xmlNodePtr source = xmlNewChild( mesh, NULL, BAD_CAST "source", NULL );
                xmlNewProp( source, BAD_CAST "id", BAD_CAST positions.id.c_str() );

                std::string positions_array;

                std::vector< float >::const_iterator p_it = positions.float_array.begin();
                std::vector< float >::const_iterator p_end = positions.float_array.end();
                for ( ; p_it != p_end; ++p_it )
                {
                    char pos[32];
                    sprintf( pos, "%f ", *p_it );

                    positions_array += pos;
                }

                char ver_count_str[16];
                char pos_count_str[16];
                sprintf( ver_count_str, "%i", vertexCount );
                sprintf( pos_count_str, "%i", positions.float_array.size() );

                xmlNodePtr float_array = xmlNewChild( source, NULL, BAD_CAST "float_array", BAD_CAST positions_array.c_str() );
                xmlNewProp( float_array, BAD_CAST "id", BAD_CAST positions.float_array_id.c_str() );
                xmlNewProp( float_array, BAD_CAST "count", BAD_CAST pos_count_str );

                std::string arrayIDRef = "#";
                arrayIDRef += positions.float_array_id;

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
                const COLLADAGeometrySource& normals = it->normals;
                
                xmlNodePtr source = xmlNewChild( mesh, NULL, BAD_CAST "source", NULL );
                xmlNewProp( source, BAD_CAST "id", BAD_CAST normals.id.c_str() );

                std::string normals_array;

                std::vector< float >::const_iterator n_it = normals.float_array.begin();
                std::vector< float >::const_iterator n_end = normals.float_array.end();
                for ( ; n_it != n_end; ++n_it )
                {
                    char nor[32];
                    sprintf( nor, "%f ", *n_it );

                    normals_array += nor;
                }

                char ver_count_str[16];
                char nor_count_str[16];
                sprintf( ver_count_str, "%i", vertexCount );
                sprintf( nor_count_str, "%i", normals.float_array.size() );

                xmlNodePtr float_array = xmlNewChild( source, NULL, BAD_CAST "float_array", BAD_CAST normals_array.c_str() );
                xmlNewProp( float_array, BAD_CAST "id", BAD_CAST normals.float_array_id.c_str() );
                xmlNewProp( float_array, BAD_CAST "count", BAD_CAST nor_count_str );

                std::string arrayIDRef = "#";
                arrayIDRef += normals.float_array_id;

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
                const COLLADAGeometrySource& texcoords = it->texcoords;
                
                xmlNodePtr source = xmlNewChild( mesh, NULL, BAD_CAST "source", NULL );
                xmlNewProp( source, BAD_CAST "id", BAD_CAST texcoords.id.c_str() );

                std::string texcoords_array;

                std::vector< float >::const_iterator t_it = texcoords.float_array.begin();
                std::vector< float >::const_iterator t_end = texcoords.float_array.end();
                for ( ; t_it != t_end; ++t_it )
                {
                    char tex[32];
                    sprintf( tex, "%f ", *t_it );

                    texcoords_array += tex;
                }

                char ver_count_str[16];
                char uvs_count_str[16];
                sprintf( ver_count_str, "%i", vertexCount );
                sprintf( uvs_count_str, "%i", texcoords.float_array.size() );

                xmlNodePtr float_array = xmlNewChild( source, NULL, BAD_CAST "float_array", BAD_CAST texcoords_array.c_str() );
                xmlNewProp( float_array, BAD_CAST "id", BAD_CAST texcoords.float_array_id.c_str() );
                xmlNewProp( float_array, BAD_CAST "count", BAD_CAST uvs_count_str );

                std::string arrayIDRef = "#";
                arrayIDRef += texcoords.float_array_id;

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
                
            xmlNodePtr vertices_node = xmlNewChild( mesh, NULL, BAD_CAST "vertices", NULL );
            xmlNewProp( vertices_node, BAD_CAST "id", BAD_CAST it->vertices_id.c_str() );

            std::string positionsIDRef = "#";
            positionsIDRef += it->positions.id;

            xmlNodePtr input_node = xmlNewChild( vertices_node, NULL, BAD_CAST "input", NULL );
            xmlNewProp( input_node, BAD_CAST "semantic", BAD_CAST "POSITION" );
            xmlNewProp( input_node, BAD_CAST "source", BAD_CAST positionsIDRef.c_str() );

            // polylists
            {
<<<<<<< HEAD
                std::vector< COLLADAGeometryPolylist >::const_iterator p_it = it->polylists.begin();
                std::vector< COLLADAGeometryPolylist >::const_iterator p_end = it->polylists.end();
                for ( ; p_it != p_end; ++p_it )
                {
                    size_t count = p_it->p.size() / 3;
                    char poly_count_str[16];
                    sprintf( poly_count_str, "%i", count );

                    xmlNodePtr polylist = xmlNewChild( mesh, NULL, BAD_CAST "polylist", NULL );
                    xmlNewProp( polylist, BAD_CAST "material", BAD_CAST p_it->material.c_str() );
                    xmlNewProp( polylist, BAD_CAST "count", BAD_CAST poly_count_str );
=======
                const BSPFace * face = faces + model->face;
                const BSPFace * face_end = faces + model->face + model->n_faces;
                for ( ; face != face_end; ++face )
                {
                    if ( face->type == 1 || face->type == 3 )
                    {
                        std::vector< int > index_buffer;

                        const BSPMeshvert * meshvert = meshverts + face->meshvert;
                        const BSPMeshvert * meshvert_end = meshverts + face->meshvert + face->n_meshverts;
                        for ( ; meshvert != meshvert_end; ++meshvert )
                        {
                            index_buffer.push_back( face->vertex + meshvert->offset );
                        }

                        if ( ! index_buffer.empty() )
                        {
                            const BSPTexture * texture = textures + face->texture;

                            char strID[64];
                            MakeStringID( texture->name, strID );

                            std::string materialID = strID;
                            materialID += "-material";
>>>>>>> 2082577e3a931bf6885ba1b3dcc2de8c9f0c638b

                    std::string verticesIDRef = "#";
                    verticesIDRef += it->vertices_id;

<<<<<<< HEAD
                    xmlNodePtr input_vertices = xmlNewChild( polylist, NULL, BAD_CAST "input", NULL );
                    xmlNewProp( input_vertices, BAD_CAST "semantic", BAD_CAST "VERTEX" );
                    xmlNewProp( input_vertices, BAD_CAST "source", BAD_CAST verticesIDRef.c_str() );
                    xmlNewProp( input_vertices, BAD_CAST "offset", BAD_CAST "0" );
=======
                            xmlNodePtr polylist = xmlNewChild( mesh, NULL, BAD_CAST "polylist", NULL );
                            xmlNewProp( polylist, BAD_CAST "material", BAD_CAST materialID.c_str() );
                            xmlNewProp( polylist, BAD_CAST "count", BAD_CAST poly_count_str );
>>>>>>> 2082577e3a931bf6885ba1b3dcc2de8c9f0c638b

                    std::string normalsIDRef = "#";
                    normalsIDRef += it->normals.id;

                    xmlNodePtr input_normals = xmlNewChild( polylist, NULL, BAD_CAST "input", NULL );
                    xmlNewProp( input_normals, BAD_CAST "semantic", BAD_CAST "NORMAL" );
                    xmlNewProp( input_normals, BAD_CAST "source", BAD_CAST normalsIDRef.c_str() );
                    xmlNewProp( input_normals, BAD_CAST "offset", BAD_CAST "1" );

                    std::string texcoordsIDRef = "#";
                    texcoordsIDRef += it->texcoords.id;

                    xmlNodePtr input_texcoords = xmlNewChild( polylist, NULL, BAD_CAST "input", NULL );
                    xmlNewProp( input_texcoords, BAD_CAST "semantic", BAD_CAST "TEXCOORD" );
                    xmlNewProp( input_texcoords, BAD_CAST "source", BAD_CAST texcoordsIDRef.c_str() );
                    xmlNewProp( input_texcoords, BAD_CAST "offset", BAD_CAST "2" );
                    xmlNewProp( input_texcoords, BAD_CAST "set", BAD_CAST "0" );

                    std::string vcount;
                    std::string p;

                    for ( size_t i=0; i<count; ++i )
                    {
                        vcount += "3 ";
                                
                        size_t offset = 3 * i;
                        char triangle_str[128];
                        sprintf( triangle_str, "%i %i %i %i %i %i %i %i %i ", p_it->p[offset+0], p_it->p[offset+0], p_it->p[offset+0]
                                                                            , p_it->p[offset+1], p_it->p[offset+1], p_it->p[offset+1]
                                                                            , p_it->p[offset+2], p_it->p[offset+2], p_it->p[offset+2] );

<<<<<<< HEAD
                        p += triangle_str;
=======
                                p += triangle_str;
                            }

                            xmlNewChild( polylist, NULL, BAD_CAST "vcount", BAD_CAST vcount.c_str() );
                            xmlNewChild( polylist, NULL, BAD_CAST "p", BAD_CAST p.c_str() );
                        }
>>>>>>> 2082577e3a931bf6885ba1b3dcc2de8c9f0c638b
                    }

                    xmlNewChild( polylist, NULL, BAD_CAST "vcount", BAD_CAST vcount.c_str() );
                    xmlNewChild( polylist, NULL, BAD_CAST "p", BAD_CAST p.c_str() );
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

        std::vector< COLLADAGeometry >::const_iterator it = library_geometries.begin();
        std::vector< COLLADAGeometry >::const_iterator end = library_geometries.end();
        for ( ; it != end; ++it )
        {
            xmlNodePtr node = xmlNewChild( visual_scene, NULL, BAD_CAST "node", NULL );
            xmlNewProp( node, BAD_CAST "id", BAD_CAST it->id.c_str() );
            xmlNewProp( node, BAD_CAST "name", BAD_CAST it->name.c_str() );
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
            geometryIDRef += it->id;

            xmlNodePtr instance_geometry = xmlNewChild( node, NULL, BAD_CAST "instance_geometry", NULL );
            xmlNewProp( instance_geometry, BAD_CAST "url", BAD_CAST geometryIDRef.c_str() );

            xmlNodePtr bind_material = xmlNewChild( instance_geometry, NULL, BAD_CAST "bind_material", NULL );
            xmlNodePtr technique_common = xmlNewChild( bind_material, NULL, BAD_CAST "technique_common", NULL );

            std::vector< COLLADAMaterial >::const_iterator m_it = library_materials.begin();
            std::vector< COLLADAMaterial >::const_iterator m_end = library_materials.end();
            for ( ; m_it != m_end; ++m_it )
            {
                std::string materialIDRef = "#";
                materialIDRef += m_it->id;

                xmlNodePtr instance_material = xmlNewChild( technique_common, NULL, BAD_CAST "instance_material", NULL );
                xmlNewProp( instance_material, BAD_CAST "symbol", BAD_CAST m_it->id.c_str() );
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
    size_t tesselation_lvl = 4;
    tesselation_steps = static_cast<size_t>(pow( 2.0f, static_cast<float>(tesselation_lvl) ));

    if ( ! RootPath( inFilename, root_path ) )
        return false;

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
