#include "MaterialCompiler/MaterialCompiler.h"

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

bool FindFiles( const char * searchStr, std::vector< std::string >& fileNames, bool absolute )
{
    WIN32_FIND_DATA ffd;
    HANDLE hFind;

    hFind = FindFirstFile( searchStr, &ffd );
    if ( hFind == INVALID_HANDLE_VALUE ) 
    {
        return false;
    }

    char dir[ 256 ];
    char * fileStart = dir;

    if ( absolute )
    {
        if ( GetFullPathName( searchStr, 256, dir, &fileStart ) == 0 )
        {
            return false;
        }
    }

    do
    {
        if ( ! ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
        {
            std::string f( dir, fileStart - dir );
            f += ffd.cFileName;
            fileNames.push_back( f );
        }
    }
    while ( FindNextFile( hFind, &ffd ) != 0 );

    FindClose( hFind );

    return true;
}

/*enum ParseState
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
            } else if ( strcmp( semantic, "COLOR" ) == 0 ) {
                input.semantic = COLOR;
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
                if ( *it == ' ' ) { *it = 0; }
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
                if ( *it == ' ' ) { *it = 0; }
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

const VType vType[COUNT][CT_COUNT] =
{
    { VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   , VT_FLOAT  , VT_HALF   },
    { VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT , VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT },
    { VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT , VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT },
    { VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT , VT_FLOAT  , VT_FLOAT  , VT_NMVECT , VT_NMVECT },
    { VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  , VT_UBYTE  },
    { VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_SHORT  , VT_SHORT  , VT_SHORT  , VT_SHORT  },
    { VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_FLOAT  , VT_SHORT  , VT_SHORT  , VT_SHORT  , VT_SHORT  },
};

const unsigned int vSize[COUNT][CT_COUNT] =
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

struct MeshInput
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

float FormatFloat( float v )
{
    float t = floor( v + 0.5f );
    if ( abs(v - t) < ( 1.0f / 4096.0f ) )
        return t;
    else
        return v;
}

void FormatData( void * dest, const float * src, int stride, const MeshInput& input )
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

std::vector< float >::iterator RemoveDuplicatedVertices( std::vector< float >& vertices, size_t stride, std::vector< size_t >& indices )
{
    std::vector< float >::iterator end = vertices.begin();

    for ( std::vector< float >::iterator vertex = vertices.begin(); vertex != vertices.end(); vertex += stride )
    {
        std::vector< float >::iterator d = vertices.begin();
        while ( d != end && !std::equal( d, d + stride, vertex ) )
        {
            d += stride;
        }

        indices.push_back( ( d - vertices.begin() ) / stride );

        if ( d == end )
        {
            std::copy( vertex, vertex + stride, end );
            end += stride;
        }
    }

    return end;
}

void OptimizeIndices( std::vector< size_t >& indices, size_t vertexCount )
{
    ReorderForsyth( indices, vertexCount );
}

void BuildTangentSpace( float * tangents,
                        float * binormals,
                        const float * vertices,
                        size_t vertex_count,
                        size_t stride,
                        const size_t * indices,
                        size_t index_count,
                        size_t pos_offset,
                        size_t nrm_offset,
                        size_t uvs_offset )
{
    for ( size_t i=0; i<index_count; i += 3 )
    {
        const size_t idx[3] = { indices[ i + 0 ]            , indices[ i + 1 ]              , indices[ i + 2 ]              };
        const float * v[3]  = { vertices + idx[0] * stride  , vertices + idx[1] * stride    , vertices + idx[2] * stride    };

        const float * p[3]  = { v[0] + pos_offset           , v[1] + pos_offset             , v[2] + pos_offset             };
        const float * w[3]  = { v[0] + uvs_offset           , v[1] + uvs_offset             , v[2] + uvs_offset             };
        float * t[3]        = { tangents + idx[0] * 3       , tangents + idx[1] * 3         , tangents + idx[2] * 3         };
        float * b[3]        = { binormals + idx[0] * 3      , binormals + idx[1] * 3        , binormals + idx[2] * 3        };

        float dx1 = p[1][0] - p[0][0];
        float dx2 = p[2][0] - p[0][0];
        float dy1 = p[1][1] - p[0][1];
        float dy2 = p[2][1] - p[0][1];
        float dz1 = p[1][2] - p[0][2];
        float dz2 = p[2][2] - p[0][2];

        float du1 = w[1][0] - w[0][0];
        float du2 = w[2][0] - w[0][0];
        float dv1 = w[1][1] - w[0][1];
        float dv2 = w[2][1] - w[0][1];

        float r = 1.0f / ( du2 * dv1 - dv2 * du1 );

        float tdir[3];
        tdir[0] = ( dv1 * dx2 - dv2 * dx1 ) * r;
        tdir[1] = ( dv1 * dy2 - dv2 * dy1 ) * r;
        tdir[2] = ( dv1 * dz2 - dv2 * dz1 ) * r;

        float bdir[3];
        bdir[0] = ( du2 * dx1 - du1 * dx2 ) * r;
        bdir[1] = ( du2 * dy1 - du1 * dy2 ) * r;
        bdir[2] = ( du2 * dz1 - du1 * dz2 ) * r;

        for ( size_t j=0; j<3; ++j )
        {
            t[j][0] += tdir[0];
            t[j][1] += tdir[1];
            t[j][2] += tdir[2];

            b[j][0] += bdir[0];
            b[j][1] += bdir[1];
            b[j][2] += bdir[2];
        }
    }

    for ( size_t i=0; i<vertex_count; ++i )
    {        
        float *         t = tangents + i * 3;
        float *         b = binormals + i * 3;
        const float *   n = vertices + i * stride + nrm_offset;

        float proj = t[0] * n[0] + t[1] * n[1] + t[2] * n[2];

        float ortho1[3];
        ortho1[0] = t[0] - n[0] * proj;
        ortho1[1] = t[1] - n[1] * proj;
        ortho1[2] = t[2] - n[2] * proj;

        float len = sqrt( ortho1[0] * ortho1[0] + ortho1[1] * ortho1[1] + ortho1[2] * ortho1[2] );
        if ( len > 0.0f )
        {
            ortho1[0] /= len;
            ortho1[1] /= len;
            ortho1[2] /= len;
        }

        float ortho2[3];
        ortho2[0] = n[1] * ortho1[2] - n[2] * ortho1[1];
        ortho2[1] = n[2] * ortho1[0] - n[0] * ortho1[2];
        ortho2[2] = n[0] * ortho1[1] - n[1] * ortho1[0];

        proj = b[0] * ortho2[0] + b[1] * ortho2[1] + b[2] * ortho2[2];
        if ( proj < 0.0f )
        {
            ortho2[0] = -ortho2[0];
            ortho2[1] = -ortho2[1];
            ortho2[2] = -ortho2[2];
        }

        memcpy( t, ortho1, sizeof( ortho1 ) );
        memcpy( b, ortho2, sizeof( ortho2 ) );
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

    size_t v_source = -1;
    for ( size_t i=0; i< sources.size(); ++i )
    {
        if ( strcmp( sources[ i ].id, v_input.source + 1 ) == 0 )
        {
            v_source = i;
            break;
        }
    }

    if ( v_source == -1 )
        return false;

    // build vertex layout
    std::vector< Semantic > semantic_layout;
    std::vector< size_t >   source_layout;
    size_t vertex_count     = 0;
    size_t vertex_stride    = 0;

    int compression = CompressionMask & options;

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

            size_t source = -1;
            for ( size_t j=0; j<sources.size(); ++j )
            {
                if ( strcmp( vertices.id, inputs[ i ].source + 1 ) == 0 )
                {
                    source = v_source;
                    break;
                }
                else if ( strcmp( sources[ j ].id, inputs[ i ].source + 1 ) == 0 )
                {
                    source = j;
                    break;
                }
            }

            if ( source == -1 )
                return false;

            if ( source_layout.size() <= i )             // Create layout on first inputs
            {
                vertex_stride += sources[ source ].stride;

                semantic_layout.push_back( inputs[ i ].semantic );
                source_layout.push_back( source );
            }
            else if ( source_layout[ i ] != source )    // check layout
            {
                return false;
            }
        }
    }

    // build triangle lists
    std::vector< float > vertex_cache;
    vertex_cache.reserve( vertex_count * vertex_stride );
    
    std::vector< size_t > sub_meshes;
    
    poly_it = polylists.begin();
    for ( ; poly_it != poly_end; ++poly_it )
    {
        sub_meshes.push_back( poly_it->count * 3 ); // Only triangles

        int * index = poly_it->primitives.data();
        int * index_end = index + poly_it->primitives.size();

        while ( index != index_end )
        {
            for ( size_t i=0; i<source_layout.size(); ++i, ++index )
            {
                const ColladaSource& source = sources[ source_layout[ i ] ];
                for ( int i=0; i<source.stride; ++i )
                    vertex_cache.push_back( FormatFloat( source.array[ i + *index * source.stride ] ) );
            }
        }
    }

    // remove duplicated vertices
    std::vector< size_t > indices;
    indices.reserve( vertex_count );

    std::vector< float >::iterator cache_end = RemoveDuplicatedVertices( vertex_cache, vertex_stride, indices );
    vertex_count = ( cache_end - vertex_cache.begin() ) / vertex_stride;

    // Optimize indices
    OptimizeIndices( indices, vertex_count );

    // Compute Tangent Space
    if ( options & OT_GENERATE_TANGENT_SPACE )
    {
        size_t pos_offset = -1;
        size_t nrm_offset = -1;
        size_t uvs_offset = -1;

        size_t offset = 0;
        for ( size_t i=0; i<semantic_layout.size(); ++i )
        {
            if ( semantic_layout[ i ] == POSITION )
                pos_offset = offset;
            else if ( semantic_layout[ i ] == NORMAL )
                nrm_offset = offset;
            else if ( semantic_layout[ i ] == TEXCOORD0 )
                uvs_offset = offset;
            offset += sources[ source_layout[ i ] ].stride;
        }

        if ( pos_offset != -1 && nrm_offset != -1 && uvs_offset != -1 )
        {
            sources.resize( sources.size() + 2 );

            std::vector< ColladaSource >::iterator tangentSource = sources.end() - 2;
            strcpy( tangentSource->id, "TANGENT" );
            tangentSource->stride = 3;
            tangentSource->array.resize( 3 * vertex_count, 0.0f );
            size_t tan_offset = offset;

            offset += tangentSource->stride;

            std::vector< ColladaSource >::iterator binormalSource = sources.end() - 1;
            strcpy( binormalSource->id, "BINORMAL" );
            binormalSource->stride = 3;
            binormalSource->array.resize( 3 * vertex_count, 0.0f );
            size_t bin_offset = offset;

            offset += binormalSource->stride;

            BuildTangentSpace( tangentSource->array.data(), binormalSource->array.data(), vertex_cache.data(), vertex_count, vertex_stride, indices.data(), indices.size(), pos_offset, nrm_offset, uvs_offset );

            // insert TS into vertex cache
            size_t cache_size = vertex_count * offset;
            vertex_cache.reserve( cache_size );

            std::vector< float > tmp( cache_size );

            std::vector< float >::iterator tmp_it   = tmp.begin();
            std::vector< float >::iterator v_it     = vertex_cache.begin();
            std::vector< float >::iterator t_it     = tangentSource->array.begin();
            std::vector< float >::iterator b_it     = binormalSource->array.begin();
            while ( tmp_it != tmp.end() )
            {
                std::copy( v_it, v_it + vertex_stride, tmp_it );
                v_it += vertex_stride;
                tmp_it += vertex_stride;

                std::copy( t_it, t_it + 3, tmp_it );
                t_it += 3;
                tmp_it += 3;

                std::copy( b_it, b_it + 3, tmp_it );
                b_it += 3;
                tmp_it += 3;
            }

            vertex_cache = tmp;

            semantic_layout.push_back( TANGENT );
            source_layout.push_back( sources.size() - 2 );

            semantic_layout.push_back( BINORMAL );
            source_layout.push_back( sources.size() - 1 );
            
            vertex_stride = offset;
            cache_end = vertex_cache.begin() + vertex_count * vertex_stride;
        }
    }

    // Build input layout
    size_t vertex_size = 0;
    std::vector< MeshInput > input_layout;

    for ( size_t i=0; i<source_layout.size(); ++i )
    {
        MeshInput input;
        input.semantic = semantic_layout[ i ];
        input.type      = vType[ input.semantic ][ compression ];
        input.size      = vSize[ input.semantic ][ compression ];
        input.offset    = vertex_size;
        vertex_size    += input.size;

        input_layout.push_back( input );
    }

    // Build vertex data
    size_t vbuffer_size = vertex_count * vertex_size;
    unsigned char * vertex_data = (unsigned char *)malloc( vbuffer_size );
    unsigned char * vertex_end = vertex_data;

    for ( size_t i=0; i<vertex_count; ++i )
    {
        float * vertex = vertex_cache.data() + i * vertex_stride;

        for ( size_t j=0; j<input_layout.size(); ++j )
        {
            MeshInput& input = input_layout[ j ];
            FormatData( vertex_end, vertex, sources[ source_layout[ j ] ].stride, input );

            vertex_end += input.size;
            vertex += sources[ source_layout[ j ] ].stride;
        }
    }

    header.inputCount       = input_layout.size();
    header.subMeshCount     = sub_meshes.size();
    header.vertexDataSize   = vbuffer_size;

    size_t index_size = 0;
    size_t index_count = indices.size();
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

    size_t ibuffer_size = index_size * index_count;

    unsigned char * index_data = (unsigned char *)malloc( ibuffer_size );
    unsigned char * index_end = index_data;
    unsigned char * index_it = ((unsigned char *)indices.data());
    while ( index_it < (unsigned char *)( indices.data() + index_count ) )
    {
        memcpy( index_end, index_it, index_size );
        index_end += index_size;
        index_it += sizeof(size_t);
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
    fwrite(input_layout.data(),1,sizeof(MeshInput)*input_layout.size(),fp);
    fwrite(vertex_data,1,vbuffer_size,fp);

    index_end = (unsigned char*)index_data;
    for ( size_t i=0; i<sub_meshes.size(); ++i )
    {
        unsigned int count  = sub_meshes[ i ];
        unsigned int size   = count * index_size;

        fwrite(&count,1,sizeof(unsigned int),fp);
        fwrite(index_end,1,size,fp);

        index_end += size;
    }

    fclose(fp);

    free( index_data );
    free( vertex_data );

    return true;
}
*/

const char samplersFileName[] = "samplers.xml";

enum FilterType
{
    FT_POINT,
    FT_LINEAR
};

enum MipType
{
    MT_NONE,
    MT_POINT,
    MT_LINEAR,
};

enum WrapType
{
    WT_REPEAT,
    WT_CLAMP,
    WT_MIRROR
};

struct Sampler
{
    char id[32];

    struct data
    {
        unsigned min    : 1;
        unsigned mag    : 1;
        unsigned mip    : 2;
        unsigned wrap   : 2;
    } d;
};

std::vector< Sampler > samplerList;

void startElementSamplers( void * ctx, const xmlChar * name, const xmlChar ** atts )
{
    if ( strcmp( (const char*)name, "sampler" ) == 0 )
    {
        Sampler sampler;
        sampler.d.min   = FT_POINT;
        sampler.d.mag   = FT_POINT;
        sampler.d.mip   = MT_NONE;
        sampler.d.wrap  = WT_REPEAT;

        const xmlChar ** a = atts;
        while ( *a != NULL )
        {
            if ( strcmp( (const char *)*a, "id" ) == 0 )
            {
                strcpy( sampler.id, (const char *)*(++a) );
            }
            else if ( strcmp( (const char *)*a, "min" ) == 0 )
            {
                if ( strcmp( (const char *)*(++a), "linear" ) == 0 )
                    sampler.d.min = FT_LINEAR;
            }
            else if ( strcmp( (const char *)*a, "mag" ) == 0 )
            {
                if ( strcmp( (const char *)*(++a), "linear" ) == 0 )
                    sampler.d.mag = FT_LINEAR;
            }
            else if ( strcmp( (const char *)*a, "mip" ) == 0 )
            {
                const char * str = (const char *)*(++a);
                if ( strcmp( str, "linear" ) == 0 )
                    sampler.d.mip = MT_LINEAR;
                else if ( strcmp( str, "point" ) == 0 )
                    sampler.d.mip = MT_POINT;
            }
            else if ( strcmp( (const char *)*a, "wrap" ) == 0 )
            {
                const char * str = (const char *)*(++a);
                if ( strcmp( str, "mirror" ) == 0 )
                    sampler.d.wrap = WT_MIRROR;
                else if ( strcmp( str, "clamp" ) == 0 )
                    sampler.d.wrap = WT_CLAMP;
            }

            ++a;
        }

        samplerList.push_back( sampler );
    }
}

bool LoadSamplerList( const char * inDir )
{
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
        &startElementSamplers,  //startElementSAXFunc startElement;
        NULL,                   //endElementSAXFunc endElement;
        NULL,                   //referenceSAXFunc reference;
        NULL,                   //charactersSAXFunc characters;
        NULL,                   //ignorableWhitespaceSAXFunc ignorableWhitespace;
        NULL,                   //processingInstructionSAXFunc processingInstruction;
        NULL,                   //commentSAXFunc comment;
        NULL,                   //warningSAXFunc warning;
        NULL,                   //errorSAXFunc error;
        NULL                    //fatalErrorSAXFunc fatalError;
    };

    std::string filename = inDir;
    filename += samplersFileName;

    int success = xmlSAXUserParseFile( &SAXHandler, NULL, filename.c_str() );

    xmlCleanupParser();
    xmlMemoryDump();
    
    return success == 0;
}

enum ParseState
{
    START,
        DESCRIPTION,
        SETS,
            SET,
                VALUE,
    FINISH
};

void startElementPrograms( void * ctx, const xmlChar * name, const xmlChar ** atts )
{
    ParseState& state = *reinterpret_cast< ParseState * >( ctx );

    switch ( state )
    {
    case START :
        if ( strcmp( (const char*)name, "description" ) == 0 )
        {
            state = DESCRIPTION;
        }
        else if ( strcmp( (const char*)name, "sets" ) == 0 )
        {
            state = SETS;
        }
        break;
    case DESCRIPTION :
        if ( strcmp( (const char*)name, "texture" ) == 0 )
        {
        }
        else if ( strcmp( (const char*)name, "parameter" ) == 0 )
        {
        }
        break;
    case SETS :
        if ( strcmp( (const char*)name, "set" ) == 0 )
        {
            state = SET;
        }
        break;
    case SET :
        if ( strcmp( (const char*)name, "value" ) == 0 )
        {
            state = VALUE;
        }
        else if ( strcmp( (const char*)name, "phong" ) == 0 )
        {
        }
        break;
    case FINISH :
        if ( strcmp( (const char*)name, "program" ) == 0 )
        {
            state = START;
        }
        break;
    }
}

std::string programs_characters;

void charactersPrograms( void * ctx, const xmlChar * name, int len )
{
    ParseState& state = *reinterpret_cast< ParseState * >( ctx );

    switch ( state )
    {
    case VALUE :
        programs_characters.append( (const char*)name, len );
        break;
    }
}

void endElementPrograms( void * ctx, const xmlChar * name )
{
    ParseState& state = *reinterpret_cast< ParseState * >( ctx );

    switch ( state )
    {
    case START :
        if ( strcmp( (const char*)name, "program" ) == 0 )
        {
            state = FINISH;
        }
        break;
    case DESCRIPTION :
        if ( strcmp( (const char*)name, "description" ) == 0 )
        {
            state = START;
        }
        break;
    case SETS :
        if ( strcmp( (const char*)name, "sets" ) == 0 )
        {
            state = START;
        }
        break;
    case SET :
        if ( strcmp( (const char*)name, "set" ) == 0 )
        {
            state = SETS;
        }
        break;
    case VALUE :
        if ( strcmp( (const char*)name, "value" ) == 0 )
        {
            state = SET;
        }
        break;
    case FINISH :
        break;
    }
}

bool LoadProgramSets( const char * inDir )
{
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
        &startElementPrograms,  //startElementSAXFunc startElement;
        &endElementPrograms,    //endElementSAXFunc endElement;
        NULL,                   //referenceSAXFunc reference;
        &charactersPrograms,    //charactersSAXFunc characters;
        NULL,                   //ignorableWhitespaceSAXFunc ignorableWhitespace;
        NULL,                   //processingInstructionSAXFunc processingInstruction;
        NULL,                   //commentSAXFunc comment;
        NULL,                   //warningSAXFunc warning;
        NULL,                   //errorSAXFunc error;
        NULL                    //fatalErrorSAXFunc fatalError;
    };

    ParseState state = FINISH;

    std::string search_str = inDir;
    search_str += "*.xml";

    std::vector< std::string > filenames;
    if ( !FindFiles( search_str.c_str(), filenames, true ) )
        return false;

    std::vector< std::string >::const_iterator it = filenames.begin();
    std::vector< std::string >::const_iterator end = filenames.end();
    for ( ; it != end; ++it )
    {
        const char * filename = it->c_str();
        if ( strcmp( samplersFileName, filename + it->length() - strlen(samplersFileName) ) == 0 )
            continue;

        if ( xmlSAXUserParseFile( &SAXHandler, &state, filename ) != 0 )
            break;
    }

    xmlCleanupParser();
    xmlMemoryDump();
    
    return it == filenames.end();
}

bool BuildMaterials( const char * outDir )
{
    return false;
}

bool CompileMaterial( const char * inDir, const char * outDir )
{
    if ( !LoadSamplerList( inDir ) )
        return false;

    if ( !LoadProgramSets( inDir ) )
        return false;

    if ( !BuildMaterials( outDir ) )
        return false;

    return true;
}
