#include "MeshCompiler/MeshCompiler.h"



#include "libxml.h"
#include "libxml/parser.h"

#include <vector>

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
    FINISH
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
                else if ( strcmp( (const char *)*a, "offset" ) == 0 )
                {
                    sscanf( (const char*)*(++a), "%d", &input.offset );
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

enum VType
{
    VT_UBYTE    = 0,
    VT_SHORT,
    VT_HALF,
    VT_FLOAT,
    VT_NMVECT      // 2_10_10_10
};

const VType vType[COUNT][OT_COMPRESS_COUNT] =
{
    { VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   },
    { VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT , VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT },
    { VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT , VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT },
    { VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT , VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT },
    { VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  },
    { VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_SHORT  , VT_SHORT  , VT_SHORT  , VT_SHORT  },
    { VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_SHORT  , VT_SHORT  , VT_SHORT  , VT_SHORT  },
};

const unsigned int vSize[COUNT][OT_COMPRESS_COUNT] =
{
    { 12    , 6     , 12    , 6     , 12    , 6     , 12    , 6 },
    { 12    , 12    , 4     , 4     , 12    , 12    , 4     , 4 },
    { 12    , 12    , 4     , 4     , 12    , 12    , 4     , 4 },
    { 12    , 12    , 4     , 4     , 12    , 12    , 4     , 4 },
    { 4     , 4     , 4     , 4     , 4     , 4     , 4     , 4 },
    { 8     , 8     , 8     , 8     , 4     , 4     , 4     , 4 },
    { 8     , 8     , 8     , 8     , 4     , 4     , 4     , 4 },
};

enum IType
{
    IT_UBYTE   = 0,
    IT_USHORT,
    IT_ULONG
};

struct MeshHeader
{
    IType        indexType;
    unsigned int subMeshCount;
    unsigned int inputCount;
    unsigned int vertexDataSize;
};

struct MeshInputDesc
{
    Semantic        semantic;
    VType           type;
    unsigned int    size;
    unsigned int    offset;
};

unsigned char toUByte( float v )
{
    return (unsigned char)( 255.0f * v );
}

// linear scale from [-256.0;256[ to [-32768;32767]
short toShort( float v )
{
    return (short)( 128.0f * v );
}

// linear scale from [-1.0;1.0] to [-512;511]
unsigned int toS10( float v )
{
    if ( v < -1.0 )
        return -512;
    if ( v > 1.0 )
        return 511;

    return ( 0x200 + (unsigned int)( v * 511.5f + 511.5f ) ) & 0x3ff;
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
unsigned short toFloat16( float v )
{
    unsigned int f_mem      = *((unsigned int *)( &v ));
    int f_sign     = ( f_mem & 0x80000000 ) >> 31;
    int f_exponent = ( f_mem & 0x7f800000 ) >> 23;
    int f_mantissa = ( f_mem & 0x07ffffff );

    unsigned short sign = f_sign;
    unsigned short exponent, mantissa;

    if ( f_exponent < 102 )
    {
        exponent = 0x0000;
        mantissa = 0x0000;
    }
    else if ( f_exponent > 144 )
    {
        exponent = 0x001f;
        mantissa = 0x03ff;
    }
    else
    {
        exponent = ( f_exponent - 112 ) & 0x001f;
        mantissa = ( f_mantissa >> 13 ) & 0x03ff;
    }

    return ( ( sign << 15 ) | ( exponent << 10 ) | mantissa );
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
    case VT_SHORT:
        {
            assert( input.semantic == TEXCOORD0 || input.semantic == TEXCOORD1 );

            short * uvs = (short * )dest;
            for ( int i=0; i<stride; ++i )
            {
                uvs[ i ] = toShort( src[i] );
            }
        }
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
    case VT_NMVECT:
        {
            assert( input.semantic == NORMAL || input.semantic == TANGENT || input.semantic == BINORMAL );

            unsigned int * n = (unsigned int *)dest;
            for ( int i=0; i<stride; ++i )
            {
                *n |= toS10( src[i] ) << ( 10 * i );
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
            size_t offset = inputs[ i ].offset;
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

    unsigned int vbuffer_size = vertex_size * vertex_count;
    void * vertex_data = malloc( vbuffer_size );
    
    unsigned char * vertex_end = (unsigned char *)vertex_data;
    std::vector< unsigned int > sub_meshes;
    
    poly_it = polylists.begin();
    for ( ; poly_it != poly_end; ++poly_it )
    {
        sub_meshes.push_back( poly_it->primitives.size() );

        int * index = poly_it->primitives.data();
        int * index_end = index + poly_it->primitives.size();

        while ( index != index_end )
        {
            for ( size_t i=0; i<source_layout.size(); ++i, ++index )
            {
                const ColladaSource * source = source_layout[ i ];
                const MeshInputDesc& input = input_layout[ i ];

                FormatData( vertex_end + input.offset, source->array.data() + *index * source->stride, source->stride, input );
            }

            vertex_end += vertex_size;
        }
    }

    // remove duplicated vertices
    vertex_end = (unsigned char *)vertex_data;
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

    vbuffer_size = ( vertex_end - (unsigned char *)vertex_data );
    vertex_count = vbuffer_size / vertex_size;

    header.vertexDataSize = vbuffer_size;

    unsigned int index_size = 0;
    unsigned int index_count = indices.size();
    if ( vertex_count <= ( 1 << 8 ) ) {
        header.indexType = IT_UBYTE;
        index_size = 1;
    } else if ( vertex_count <= ( 1 << 16 ) ) {
        header.indexType = IT_USHORT;
        index_size = 2;
    } else {
        header.indexType = IT_ULONG;
        index_size = 4;
    }

    unsigned int ibuffer_size = index_size * index_count;

    void * index_data = malloc( ibuffer_size );
    unsigned char * index_end = (unsigned char *)index_data;
    unsigned char * index_it = ((unsigned char *)indices.data()) + ( sizeof(unsigned int) - index_size );
    while ( index_it < (unsigned char *)( indices.data() + index_count ) )
    {
        memcpy( index_end, index_it, index_size );
        index_end += index_size;
        index_it += sizeof(unsigned int);
    }

    if ( ! BuildDirectory( outFilename ) )
    {
        free( index_data );
        free( vertex_data );
        return false;
    }

    FILE *fp;

    if (fopen_s(&fp, outFilename, "wb"))
    {
        free( index_data );
        free( vertex_data );
        return false;
    }

    fwrite(&header,1,sizeof(MeshHeader),fp);
    fwrite(input_layout.data(),1,sizeof(MeshInputDesc)*input_layout.size(),fp);
    fwrite(sub_meshes.data(),1,sizeof(unsigned int)*sub_meshes.size(),fp);
    fwrite(vertex_data,1,vbuffer_size,fp);
    fwrite(index_data,1,ibuffer_size,fp);

    fclose(fp);

    free( index_data );
    free( vertex_data );

    return true;
}

bool CompileMesh( const char * inFilename, const char * outFilename, int options )
{
    LoadCollada( inFilename );

    BuildMesh( outFilename, options );

    return true;
}
