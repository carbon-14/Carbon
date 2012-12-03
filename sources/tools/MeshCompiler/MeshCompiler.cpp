#include "MeshCompiler/MeshCompiler.h"

#include "libxml.h"
#include "libxml/parser.h"

#include <vector>

enum ParseState
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
    POSITION    = 0,
    NORMAL      = 1,
    TEXCOORD    = 2
};

struct ColladaAccessor
{
    int count;
    int stride;
};

struct ColladaSource
{
    char                    id[32];
    int                     stride;
    std::vector< float >    array;
};

struct ColladaInput
{
    Semantic    semantic;
    int         offset;
    char        source[32];
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

void startElementCollada( void * ctx, const xmlChar * name, const xmlChar ** atts )
{
    ParseState& state = *reinterpret_cast< ParseState * >( ctx );

    switch ( state )
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
            state = GEOMETRY;
        }
        break;
    case GEOMETRY :
        if ( strcmp( (const char*)name, "mesh" ) == 0 )
        {
            state = MESH;
        }
        break;
    case MESH :
        if ( strcmp( (const char*)name, "source" ) == 0 )
        {
            ColladaSource source;
            *source.id = 0;
            source.stride = 0;

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

            state = SOURCE;
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
            
            state = VERTICES;
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

            state = POLYLIST;
        }
        break;
    case SOURCE :
        if ( strcmp( (const char*)name, "float_array" ) == 0 )
        {
            state = ARRAY;
        }
        else if ( strcmp( (const char*)name, "accessor" ) == 0 )
        {
            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "stride" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%d", &(geometry.mesh.sources.back().stride) );
                }
                else
                {
                    ++a;
                }
                ++a;
            }
        }
        break;
    case VERTICES :
        if ( strcmp( (const char*)name, "input" ) == 0 )
        {
            ColladaInput input;
            input.semantic = POSITION;
            input.offset = 0;
            *input.source = 0;

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
            input.offset = 0;
            *input.source = 0;

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
            state = PRIMITIVES;
        }
        break;
    case FINISH :
        if ( strcmp( (const char*)name, "library_geometries" ) == 0 )
        {
            state = START;
        }
        break;
    default :
        state = ERROR;
    }
}

void endElementCollada( void * ctx, const xmlChar * name )
{
    ParseState& state = *reinterpret_cast< ParseState * >( ctx );

    switch ( state )
    {
    case START :
        if ( strcmp( (const char*)name, "library_geometries" ) == 0 )
        {
            state = FINISH;
        }
        break;
    case GEOMETRY :
        if ( strcmp( (const char*)name, "geometry" ) == 0 )
        {
            state = START;
        }
        break;
    case MESH :
        if ( strcmp( (const char*)name, "mesh" ) == 0 )
        {
            state = GEOMETRY;
        }
        break;
    case SOURCE :
        if ( strcmp( (const char*)name, "source" ) == 0 )
        {
            state = MESH;
        }
        break;
    case ARRAY :
        if ( strcmp( (const char*)name, "float_array" ) == 0 )
        {
            state = SOURCE;
        }
        break;
    case VERTICES :
        if ( strcmp( (const char*)name, "vertices" ) == 0 )
        {
            state = MESH;
        }
        break;
    case POLYLIST :
        if ( strcmp( (const char*)name, "polylist" ) == 0 )
        {
            state = MESH;
        }
        break;
    case PRIMITIVES :
        if ( strcmp( (const char*)name, "p" ) == 0 )
        {
            state = POLYLIST;
        }
        break;
    case FINISH :
        break;
    default :
        state = ERROR;
    }
}

void charactersCollada( void * ctx, const xmlChar * name, int len )
{
    ParseState& state = *reinterpret_cast< ParseState * >( ctx );

    switch ( state )
    {
    case ARRAY :
        {
            ColladaSource& source = geometry.mesh.sources.back();

            char * str = new char[len+1];

            memcpy( str, name, len+1 );

            char * it = str;
            const char * last = it + len;
            const char * end = last + 1;

            while ( it != end )
            {
                const char * begin = it;
                while ( *it != ' ' && it != last ) { ++it; }
                *it = 0;

                float v;
                sscanf( begin, "%f", &v );
                source.array.push_back( v );
                ++it;
            }

            delete str;
        }
        break;
    case PRIMITIVES :
        {
            ColladaPolylist& polylist = geometry.mesh.polylists.back();

            char * str = new char[len+1];

            memcpy( str, name, len+1 );

            char * it = str;
            const char * last = it + len;
            const char * end = last + 1;

            while ( it != end )
            {
                const char * begin = it;
                while ( *it != ' ' && it != last ) { ++it; }
                *it = 0;

                int v;
                sscanf( begin, "%d", &v );
                polylist.primitives.push_back( v );
                ++it;
            }

            delete str;
        }
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

int LoadCollada( const char * filename )
{
    ParseState state = FINISH;

    int success = xmlSAXUserParseFile( &SAXHandler, &state, filename );

    xmlCleanupParser();
    xmlMemoryDump();
    
    return success;
}

//////////////////////////////////////////////////////////////////////////

struct MeshHeader
{
    unsigned int vertexDataSize;
    unsigned int subMeshCount;
    unsigned int inputCount;
};

struct MeshInputDesc
{
    int semantic;
    int size;
    int offset;
};

struct SubMeshDesc
{
    int type;
    int count;
};

bool BuildMesh()
{
    MeshHeader header;

    const ColladaMesh mesh = geometry.mesh;
    const std::vector< ColladaSource > sources = mesh.sources;
    const ColladaVertices& vertices = mesh.vertices;
    const ColladaInput& v_input = vertices.inputs.front();
    const std::vector< ColladaPolylist > polylists = mesh.polylists;

    header.inputCount = sources.size();
    header.subMeshCount = polylists.size();

    const ColladaSource * v_source = NULL;
    for ( size_t i=0; i< sources.size(); ++i )
    {
        const ColladaSource& source = sources[ i ];
        if ( strcmp( source.id, v_input.source + 1 ) == 0 )
        {
            v_source = &source;
        }
    }

    if ( ! v_source )
        return false;

    std::vector< MeshInputDesc > layout;
    unsigned int vertex_size = 0;
    const std::vector< ColladaInput >& inputs = polylists.front().inputs;
    for ( size_t i=0; i<inputs.size(); ++i )
    {
        MeshInputDesc input;

        input.semantic  = inputs[ i ].semantic;
        input.size      = 4;                        // 4-bytes float no compressed for now
        input.offset    = vertex_size;

        if ( strcmp( vertices.id, inputs[ i ].source + 1 ) == 0 )
        {
            vertex_size += v_source->stride * input.size;
        }
        else
        {
            for ( size_t j=0; j<sources.size(); ++j )
            {
                if ( strcmp( sources[ j ].id, inputs[ i ].source + 1 ) == 0 )
                {
                    vertex_size += sources[ j ].stride * 4;
                }
            }
        }

        layout.push_back( input );
    }

    return true;
}

bool CompileMesh()
{
    LoadCollada( "../../../data/sibenik.dae" );

    BuildMesh();

    return true;
}
