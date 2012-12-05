#include "MeshCompiler/MeshCompiler.h"

#include "libxml.h"
#include "libxml/parser.h"

#include <vector>

#include <cassert>

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
    POSITION = 0,
    NORMAL,
    TANGENT,
    BINORMAL,
    COLOR,
    TEXCOORD0,
    TEXCOORD1,
    COUNT
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
    int                         count;
    std::vector< ColladaInput > inputs;
    std::vector< int >          primitives;     // indices
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
            geometry.mesh.sources.clear();
            geometry.mesh.polylists.clear();
            state = MESH;
        }
        break;
    case MESH :
        if ( strcmp( (const char*)name, "source" ) == 0 )
        {
            ColladaSource source;
            *source.id = 0;
            source.stride = 0;
            source.array.clear();

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
            mesh.vertices.inputs.clear();

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
            polylist.count = 0;
            polylist.inputs.clear();
            polylist.primitives.clear();

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "material" ) == 0 )
                {
                    strcpy( (char*)polylist.material, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "count" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%d", &polylist.count );
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
    case POLYLIST :
        if ( strcmp( (const char*)name, "input" ) == 0 )
        {
            ColladaInput input;
            input.semantic = POSITION;
            input.offset = 0;
            *input.source = 0;

            char semantic[32] = "";
            int set = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "semantic" ) == 0 )
                {
                    strcpy( semantic, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "source" ) == 0 )
                {
                    strcpy( (char *)input.source, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "set" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%d", &set );
                }
                else
                {
                    ++a;
                }
                ++a;
            }

            if ( strcmp( semantic, "POSITION" ) == 0 ) {
                input.semantic = POSITION;
            } else if ( strcmp( semantic, "NORMAL" ) == 0 ) {
                input.semantic = NORMAL;
            } else if ( strcmp( semantic, "TEXCOORD" ) == 0 ) {
                if ( set == 0 )
                    input.semantic = TEXCOORD0;
                else if ( set == 1 )
                    input.semantic = TEXCOORD1;
            }

            ColladaMesh& mesh = geometry.mesh;

            if ( state == VERTICES )
            {
                ColladaVertices& vertices = mesh.vertices;
                vertices.inputs.push_back( input );
            }
            else
            {
                ColladaPolylist& polylist = mesh.polylists.back();
                polylist.inputs.push_back( input );
            }
        }
        else if ( state == POLYLIST && strcmp( (const char*)name, "p" ) == 0 )
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

std::string collada_characters;

void charactersCollada( void * ctx, const xmlChar * name, int len )
{
    ParseState& state = *reinterpret_cast< ParseState * >( ctx );

    switch ( state )
    {
    case ARRAY :
    case PRIMITIVES :
        collada_characters.append( (const char*)name, len );
        break;
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
            ColladaSource& source = geometry.mesh.sources.back();

            std::string::iterator it = collada_characters.begin();
            std::string::iterator end = collada_characters.end();
            while ( it != end )
            {
                if ( *it < 48 || *it > 57 ) { *it = 0; }
                ++it;
            }

            it = collada_characters.begin();
            while ( it != end && *it == 0 ) { ++it; }

            while ( it != end )
            {
                float v;
                sscanf( &*it, "%f", &v );
                source.array.push_back( v );

                while ( it != end && *it != 0 ) { ++it; }
                while ( it != end && *it == 0 ) { ++it; }
            }

            collada_characters.clear();
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
            ColladaPolylist& polylist = geometry.mesh.polylists.back();

            std::string::iterator it = collada_characters.begin();
            std::string::iterator end = collada_characters.end();
            while ( it != end )
            {
                if ( *it < 48 || *it > 57 ) { *it = 0; }
                ++it;
            }

            it = collada_characters.begin();
            while ( it != end && *it == 0 ) { ++it; }

            while ( it != end )
            {
                int v;
                sscanf( &*it, "%d", &v );
                polylist.primitives.push_back( v );

                while ( it != end && *it != 0 ) { ++it; }
                while ( it != end && *it == 0 ) { ++it; }
            }

            collada_characters.clear();
            state = POLYLIST;
        }
        break;
    case FINISH :
        break;
    default :
        state = ERROR;
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

enum VType
{
    VT_UBYTE    = 0,
    VT_HALF,
    VT_FLOAT,
    VT_NORMAL      // 2_10_10_10
};

const VType vType[COUNT][OT_COMPRESS_COUNT] =
{
    { VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   },
    { VT_FLOAT  , VT_FLOAT  , VT_NORMAL , VT_NORMAL , VT_FLOAT  , VT_FLOAT  , VT_NORMAL , VT_NORMAL },
    { VT_FLOAT  , VT_FLOAT  , VT_NORMAL , VT_NORMAL , VT_FLOAT  , VT_FLOAT  , VT_NORMAL , VT_NORMAL },
    { VT_FLOAT  , VT_FLOAT  , VT_NORMAL , VT_NORMAL , VT_FLOAT  , VT_FLOAT  , VT_NORMAL , VT_NORMAL },
    { VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  },
    { VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_HALF   , VT_HALF   , VT_HALF   , VT_HALF   },
    { VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_HALF   , VT_HALF   , VT_HALF   , VT_HALF   },
};

const unsigned int vSize[COUNT][OT_COMPRESS_COUNT] =
{
    { 16    , 8     , 16    , 8     , 16    , 8     , 16    , 8 },
    { 12    , 12    , 4     , 4     , 12    , 12    , 4     , 4 },
    { 12    , 12    , 4     , 4     , 12    , 12    , 4     , 4 },
    { 12    , 12    , 4     , 4     , 12    , 12    , 4     , 4 },
    { 4     , 4     , 4     , 4     , 4     , 4     , 4     , 4 },
    { 8     , 8     , 8     , 8     , 4     , 4     , 4     , 4 },
    { 8     , 8     , 8     , 8     , 4     , 4     , 4     , 4 },
};

struct MeshInputDesc
{
    Semantic    semantic;
    VType       type;
    int         size;
    int         offset;
};

enum IType
{
    IT_UBYTE   = 0,
    IT_USHORT,
    IT_UINTEGER
};

struct SubMeshDesc
{
    IType   type;
    int     count;
};

unsigned char toUByte( float v )
{
    return floor( 255.0f * v );
}

// Float 32-bits
// sign     : 1 bit
// exponent : 8 bits
// mantissa : 23 bits
// if ( v < 2^-128 ) -> ( mantissa / 2^24 ) * 2^-128
// else              -> ( 1 + mantissa / 2^24 ) * 2^( exponent - 128 )

// Float 16-bits ( half )
// sign     : 1 bit
// exponent : 5 bits
// mantissa : 10 bits
// if ( v < 2^-16 ) -> (-1)^sign * ( mantissa / 2^11 ) * 2^-16
// else             -> (-1)^sign * ( 1 + mantissa / 2^11 ) * 2^( exponent - 16 )

// Float 10-bits
// exponent : 5 bits
// mantissa : 5 bits
// if ( v < 2^-16 ) -> ( mantissa / 2^6 ) * 2^-16
// else             -> ( 1 + mantissa / 2^6 ) * 2^( exponent - 16 )

unsigned short toFloat16( float v )
{
    unsigned int f_mem      = *((unsigned int *)( &v ));
    int f_sign     = ( f_mem & 0x80000000 ) >> 31;
    int f_exponent = ( f_mem & 0x7f800000 ) >> 23;
    int f_mantissa = ( f_mem & 0x07ffffff );

    unsigned short sign = f_sign;
    unsigned short exponent, mantissa;

    if ( ( f_exponent - 128 ) < -26 )
    {
        exponent = 0x0000;
        mantissa = 0x0000;
    }
    else if ( ( f_exponent - 128 ) < -16 )
    {
        exponent = 0x0000;
        mantissa = ( f_mantissa >> 13 ) & 0x0000ffff;
    }
    else if ( ( f_exponent - 128 ) > 16 )
    {
        exponent = 0x0031;
        mantissa = 0x03ff;
    }
    else
    {
        exponent = f_exponent - 112;
        mantissa = ( f_mantissa >> 13 ) & 0x0000ffff;
    }

    return ( ( sign << 15 ) | ( exponent << 10 ) | mantissa );
}

unsigned short toFloat10( float v )
{
    unsigned int f_mem      = *((unsigned int *)( &v ));
    int f_exponent = ( f_mem & 0x7f800000 ) >> 23;
    int f_mantissa = ( f_mem & 0x07ffffff );

    unsigned short exponent, mantissa;

    if ( ( f_exponent - 128 ) < -26 )
    {
        exponent = 0x0000;
        mantissa = 0x0000;
    }
    else if ( ( f_exponent - 128 ) < -16 )
    {
        exponent = 0x0000;
        mantissa = ( f_mantissa >> 18 ) & 0x0000ffff;
    }
    else if ( ( f_exponent - 128 ) > 16 )
    {
        exponent = 0x0031;
        mantissa = 0x03ff;
    }
    else
    {
        exponent = f_exponent - 112;
        mantissa = ( f_mantissa >> 18 ) & 0x0000ffff;
    }

    return ( ( exponent << 5 ) | mantissa );
}

void FormatData( void * dest, const float * src, int stride, const MeshInputDesc& input )
{
    memset( dest, 0, input.size );

    switch ( input.type )
    {
    case VT_UBYTE:
        {
            assert( input.semantic == COLOR );

            unsigned char * color = (unsigned char *)dest;
            for ( int i=0; i<stride; ++i )
            {
                color[ i ] = toUByte( src[i] );
            }
        }
        break;
    case VT_HALF:
        {
            assert( input.semantic == POSITION || input.semantic == TEXCOORD0 || input.semantic == TEXCOORD1 );

            unsigned short * half = (unsigned short *)dest;
            for ( int i=0; i<stride; ++i )
            {
                half[ i ] = toFloat16( src[i] );
            }
        }
        break;
    case VT_FLOAT:
        {
            memcpy( dest, src, stride * sizeof( float ) );
        }
        break;
    case VT_NORMAL:
        {
            assert( input.semantic == NORMAL || input.semantic == TANGENT || input.semantic == BINORMAL );

            unsigned int * n = (unsigned int *)dest;
            for ( int i=0; i<stride; ++i )
            {
                *n |= toFloat10( src[i] ) >> ( 32 - 10 * i );
            }
        }
        break;
    }
}

bool BuildMesh( const char * outFilename, int options )
{
    MeshHeader header;

    ColladaMesh mesh = geometry.mesh;
    std::vector< ColladaSource > sources = mesh.sources;
    ColladaVertices& vertices = mesh.vertices;
    ColladaInput& v_input = vertices.inputs.front();
    std::vector< ColladaPolylist > polylists = mesh.polylists;

    header.inputCount = sources.size();
    header.subMeshCount = polylists.size();

    ColladaSource * v_source = NULL;
    for ( size_t i=0; i< sources.size(); ++i )
    {
        ColladaSource& source = sources[ i ];
        if ( strcmp( source.id, v_input.source + 1 ) == 0 )
        {
            v_source = &source;
            break;
        }
    }

    if ( v_source == NULL )
        return false;

    // build vertex layout

    std::vector< MeshInputDesc >    input_layout;
    std::vector< ColladaSource * >  source_layout;
    unsigned int vertex_size = 0;
    unsigned int vertex_count = 0;

    std::vector< ColladaPolylist >::iterator poly_it = polylists.begin();
    std::vector< ColladaPolylist >::iterator poly_end = polylists.end();
    for ( ; poly_it != poly_end; ++poly_it )
    {
        vertex_count += poly_it->count * 3; // only triangles

        std::vector< ColladaInput >& inputs = poly_it->inputs;
        for ( size_t i=0; i<inputs.size(); ++i )
        {
            int offset = inputs[ i ].offset;
            while ( offset != i )
            {
                if ( offset >= inputs.size() )
                    return false;

                std::swap( inputs[ i ], inputs[ offset ] );
            }

            ColladaSource * source = NULL;
            for ( size_t j=0; j<sources.size(); ++j )
            {
                if ( strcmp( vertices.id, inputs[ i ].source + 1 ) == 0 )
                {
                    source = v_source;
                    break;
                }
                else if ( strcmp( sources[ j ].id, inputs[ i ].source + 1 ) == 0 )
                {
                    source = &sources[ j ];
                    break;
                }
            }

            if ( source == NULL )
                return false;

            if ( input_layout.size() <= i )             // Create layout on first inputs
            {
                MeshInputDesc input;
                input.semantic = inputs[ i ].semantic;
                input.type = vType[ input.semantic ][ options ];
                input.size = vSize[ input.semantic ][ options ];
                input.offset = vertex_size;

                vertex_size += input.size;

                input_layout.push_back( input );
                source_layout.push_back( source );
            }
            else if ( source_layout[ i ] != source )    // check layout
            {
                return false;
            }
        }
    }

    // build triangle lists

    std::vector< void * > sub_meshes;

    void * vertex_data = malloc( vertex_size * vertex_count );
    unsigned char * ptr = (unsigned char*)vertex_data;

    unsigned int buffer_size = vertex_size * vertex_count;
    
    poly_it = polylists.begin();
    for ( ; poly_it != poly_end; ++poly_it )
    {
        sub_meshes.push_back( ptr );

        int * index = poly_it->primitives.data();
        int * index_end = index + poly_it->primitives.size();

        while ( index != index_end )
        {
            for ( size_t i=0; i<source_layout.size(); ++i, ++index )
            {
                const ColladaSource * source = source_layout[ i ];
                const MeshInputDesc& input = input_layout[ i ];

                FormatData( ptr + input.offset, source->array.data() + *index * source->stride, source->stride, input );
            }

            ptr += vertex_size;
        } 
    }

    // remove duplicated vertices
    unsigned char * vertex_end = (unsigned char *)vertex_data;
    std::vector< unsigned int > indices;
    indices.reserve( vertex_count );
    for ( unsigned int i=0; i<vertex_count; ++i )
    {
        unsigned char * vertex = (unsigned char *)vertex_data + i * vertex_size;

        unsigned char * d = (unsigned char *)vertex_data;
        while ( d != vertex_end )
        {
            if ( memcmp( d, vertex, vertex_size ) == 0 ) { break; }
            d += vertex_size;
        }

        indices.push_back( ( d - (unsigned char *)vertex_data ) / vertex_size );

        if ( d == vertex_end )
        {
            memcpy( vertex_end, vertex, vertex_size );
            vertex_end += vertex_size;
        }
    }

    unsigned int new_buffer_size = vertex_end - (unsigned char *)vertex_data;

    free( vertex_data );

    return true;
}

bool CompileMesh( const char * inFilename, const char * outFilename, int options )
{
    LoadCollada( inFilename );

    BuildMesh( outFilename, options );

    return true;
}
