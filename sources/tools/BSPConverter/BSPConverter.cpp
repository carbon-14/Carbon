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
    unsigned int textureCount = header->dirEntries[ LTextures ].length / sizeof(BSPTexture);

    const BSPModel * models = reinterpret_cast< BSPModel * >( data + header->dirEntries[ LModels ].offset );
    unsigned int modelCount = header->dirEntries[ LModels ].length / sizeof(BSPModel);

    const BSPVertex * vertices = reinterpret_cast< BSPVertex* >( data + header->dirEntries[ LVertices ].offset );
    unsigned int vertexCount = header->dirEntries[ LVertices ].length / sizeof(BSPVertex);

    const BSPMeshvert * meshverts = reinterpret_cast< BSPMeshvert * >( data + header->dirEntries[ LMeshverts ].offset );
    unsigned int meshvertCount = header->dirEntries[ LMeshverts ].length / sizeof(BSPMeshvert);

    const BSPFace * faces = reinterpret_cast< BSPFace * >( data + header->dirEntries[ LFaces ].offset );
    unsigned int faceCount = header->dirEntries[ LFaces ].length / sizeof(BSPFace);

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

        const BSPTexture * it = textures;
        const BSPTexture * end = textures + textureCount;
        for ( ; it != end; ++it )
        {
            char strID[64];
            MakeStringID( it->name, strID );

            xmlNodePtr image = xmlNewChild( lib_images, NULL, BAD_CAST "image", NULL );
            xmlNewProp( image, BAD_CAST "id", BAD_CAST strID );
            xmlNewProp( image, BAD_CAST "name", BAD_CAST strID );
            xmlNewChild( image, NULL, BAD_CAST "init_from", BAD_CAST it->name );
        }
    }

    // library_effects
    {
        xmlNodePtr lib_effects = xmlNewChild( root, NULL, BAD_CAST "library_effects", NULL );

        const BSPTexture * it = textures;
        const BSPTexture * end = textures + textureCount;
        for ( ; it != end; ++it )
        {
            char strID[64];
            MakeStringID( it->name, strID );

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
            xmlNewChild( sampler, NULL, BAD_CAST "init_from", BAD_CAST surfaceID.c_str() );

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

        const BSPTexture * it = textures;
        const BSPTexture * end = textures + textureCount;
        for ( ; it != end; ++it )
        {
            char strID[64];
            MakeStringID( it->name, strID );

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

        int model_counter = 0;

        const BSPModel * it = models;
        const BSPModel * end = models + modelCount;
        for ( ; it != end; ++it, ++model_counter )
        {
            if ( it->n_faces == 0 )
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
                    sprintf( pos, "%f %f %f", vertices[i].position[0], vertices[i].position[1], vertices[i].position[2] );

                    if ( i > 0 )
                    {
                        positions_array += " ";
                    }

                    positions_array += pos;
                }

            }
        }
    }


    xmlChar *xmlbuff;
    int buffersize;

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    printf("%s", (char *) xmlbuff);

    /*
     * Free associated memory.
     */
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
