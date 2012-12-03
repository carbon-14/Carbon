#include "MeshCompiler/MeshCompiler.h"

#include "libxml.h"
#include "libxml/parser.h"

#include <vector>

enum StatesEnum
{
    START,
        GEOMETRY,
            MESH,
                SOURCE,
                    ARRAY,
                VERTICES,
                POLYLIST,
                    PRIMITIVES,
    FINISH,
    ERROR
};

enum Semantic
{
    POSITION,
    NORMAL,
    TEXCOORD
};

struct ColladaAccessor
{
    int count;
    int stride;
};

struct ColladaSource
{
    char                    id[32];
    std::vector< float >    array;
};

struct ColladaInput
{
    Semantic    semantic;
    char        source[32];
    int         offset;
};

struct ColladaVertices
{
    char                        id[32];
    std::vector< ColladaInput > inputs;
};

struct ColladaPolylist
{
    char                        material[32];   // not used yet
    std::vector< ColladaInput > inputs;
    std::vector< int >          primitives;     // indicies
};

struct ColladaMesh
{
    std::vector< ColladaSource >    sources;
    ColladaVertices                 vertices;
    std::vector< ColladaPolylist >  polylists;
};

struct ColladaGeometry
{
    char        id[32];
    char        name[32];
    ColladaMesh mesh;
};


///////////////////////////////////////////////////////////////////////////////////////////////

ColladaGeometry geometry;

struct ParseData
{
    StatesEnum      state;
};

void startElementCollada( void * ctx, const xmlChar * name, const xmlChar ** atts )
{
    ParseData * data = reinterpret_cast< ParseData * >( ctx );

    switch ( data->state )
    {
    case START :
        if ( strcmp( (const char*)name, "geometry" ) == 0 )
        {
            *geometry.id = 0;
            *geometry.name = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "id" ) == 0 )
                {
                    strcpy( geometry.id, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "name" ) == 0 )
                {
                    strcpy( geometry.name, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }
                ++a;
            }
            data->state = GEOMETRY;
        }
        break;
    case GEOMETRY :
        if ( strcmp( (const char*)name, "mesh" ) == 0 )
        {
            data->state = MESH;
        }
        break;
    case MESH :
        if ( strcmp( (const char*)name, "source" ) == 0 )
        {
            ColladaSource source;
            *source.id = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "id" ) == 0 )
                {
                    strcpy( source.id, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }
                ++a;
            }

            geometry.mesh.sources.push_back( source );

            data->state = SOURCE;
        }
        else if ( strcmp( (const char*)name, "vertices" ) == 0 )
        {
            ColladaMesh& mesh = geometry.mesh;
            *mesh.vertices.id = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "id" ) == 0 )
                {
                    strcpy( mesh.vertices.id, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }
                ++a;
            }
            
            data->state = VERTICES;
        }
        else if ( strcmp( (const char*)name, "polylist" ) == 0 )
        {
            ColladaPolylist polylist;
            *polylist.material = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "material" ) == 0 )
                {
                    strcpy( (char*)polylist.material, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }
                ++a;
            }

            geometry.mesh.polylists.push_back( polylist );

            data->state = POLYLIST;
        }
        break;
    case SOURCE :
        if ( strcmp( (const char*)name, "float_array" ) == 0 )
        {
            data->state = ARRAY;
        }
        break;
    case VERTICES :
        if ( strcmp( (const char*)name, "input" ) == 0 )
        {
            ColladaInput input;
            input.semantic = POSITION;
            *input.source = 0;
            input.offset = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "semantic" ) == 0 )
                {
                    const char * semantic = (const char *)*(++a);
                    if ( strcmp( semantic, "POSITION" ) == 0 ) {
                        input.semantic = POSITION;
                    } else if ( strcmp( semantic, "NORMAL" ) == 0 ) {
                        input.semantic = NORMAL;
                    } else if ( strcmp( semantic, "TEXCOORD" ) == 0 ) {
                        input.semantic = TEXCOORD;
                    }
                }
                else if ( strcmp( (const char *)*a, "source" ) == 0 )
                {
                    strcpy( (char *)input.source, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }
                ++a;
            }

            ColladaMesh& mesh = geometry.mesh;
            ColladaVertices& vertices = mesh.vertices;
            vertices.inputs.push_back( input );
        }
        break;
    case POLYLIST :
        if ( strcmp( (const char*)name, "input" ) == 0 )
        {
            ColladaInput input;
            input.semantic = POSITION;
            *input.source = 0;
            input.offset = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "semantic" ) == 0 )
                {
                    const char * semantic = (const char *)*(++a);
                    if ( strcmp( semantic, "POSITION" ) ) {
                        input.semantic = POSITION;
                    } else if ( strcmp( semantic, "NORMAL" ) ) {
                        input.semantic = NORMAL;
                    } else if ( strcmp( semantic, "TEXCOORD" ) ) {
                        input.semantic = TEXCOORD;
                    }
                }
                else if ( strcmp( (const char *)*a, "source" ) == 0 )
                {
                    strcpy( (char *)input.source, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "offset" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%d", &input.offset );
                }
                else
                {
                    ++a;
                }
                ++a;
            }

            ColladaMesh& mesh = geometry.mesh;
            ColladaPolylist& polylist = mesh.polylists.back();
            polylist.inputs.push_back( input );
        }
        else if ( strcmp( (const char*)name, "p" ) == 0 )
        {
            data->state = PRIMITIVES;
        }
        break;
    case FINISH :
        if ( strcmp( (const char*)name, "library_geometries" ) == 0 )
        {
            data->state = START;
        }
        break;
    default :
        data->state = ERROR;
    }
}

void endElementCollada( void * ctx, const xmlChar * name )
{
    ParseData * data = reinterpret_cast< ParseData * >( ctx );

    switch ( data->state )
    {
    case START :
        if ( strcmp( (const char*)name, "library_geometries" ) == 0 )
        {
            data->state = FINISH;
        }
        break;
    case GEOMETRY :
        if ( strcmp( (const char*)name, "geometry" ) == 0 )
        {
            data->state = START;
        }
        break;
    case MESH :
        if ( strcmp( (const char*)name, "mesh" ) == 0 )
        {
            data->state = GEOMETRY;
        }
        break;
    case SOURCE :
        if ( strcmp( (const char*)name, "source" ) == 0 )
        {
            data->state = MESH;
        }
        break;
    case ARRAY :
        if ( strcmp( (const char*)name, "float_array" ) == 0 )
        {
            data->state = SOURCE;
        }
        break;
    case VERTICES :
        if ( strcmp( (const char*)name, "vertices" ) == 0 )
        {
            data->state = MESH;
        }
        break;
    case POLYLIST :
        if ( strcmp( (const char*)name, "polylist" ) == 0 )
        {
            data->state = MESH;
        }
        break;
    case PRIMITIVES :
        if ( strcmp( (const char*)name, "p" ) == 0 )
        {
            data->state = POLYLIST;
        }
        break;
    case FINISH :
        break;
    default :
        data->state = ERROR;
    }
}

void charactersCollada( void * ctx, const xmlChar * name, int len )
{
    ParseData * data = reinterpret_cast< ParseData * >( ctx );

    switch ( data->state )
    {
    case ARRAY :
        // read float array data
        break;
    case PRIMITIVES :
        // read primitives data
        break;
    }
}

xmlSAXHandler SAXHandler = {
    NULL,                   //internalSubsetSAXFunc internalSubset;
    NULL,                   //isStandaloneSAXFunc isStandalone;
    NULL,                   //hasInternalSubsetSAXFunc hasInternalSubset;
    NULL,                   //hasExternalSubsetSAXFunc hasExternalSubset;
    NULL,                   //resolveEntitySAXFunc resolveEntity;
    NULL,                   //getEntitySAXFunc getEntity;
    NULL,                   //entityDeclSAXFunc entityDecl;
    NULL,                   //notationDeclSAXFunc notationDecl;
    NULL,                   //attributeDeclSAXFunc attributeDecl;
    NULL,                   //elementDeclSAXFunc elementDecl;
    NULL,                   //unparsedEntityDeclSAXFunc unparsedEntityDecl;
    NULL,                   //setDocumentLocatorSAXFunc setDocumentLocator;
    NULL,                   //startDocumentSAXFunc startDocument;
    NULL,                   //endDocumentSAXFunc endDocument;
    &startElementCollada,   //startElementSAXFunc startElement;
    &endElementCollada,     //endElementSAXFunc endElement;
    NULL,                   //referenceSAXFunc reference;
    &charactersCollada,     //charactersSAXFunc characters;
    NULL,                   //ignorableWhitespaceSAXFunc ignorableWhitespace;
    NULL,                   //processingInstructionSAXFunc processingInstruction;
    NULL,                   //commentSAXFunc comment;
    NULL,                   //warningSAXFunc warning;
    NULL,                   //errorSAXFunc error;
    NULL                    //fatalErrorSAXFunc fatalError;
};

int ParseXMLFile( const char * filename )
{
    ParseData data;
    data.state = FINISH;

    if ( xmlSAXUserParseFile( &SAXHandler, &data, filename ) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

bool CompileMesh()
{
    ParseXMLFile( "../../../data/sibenik.dae" );

    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}
