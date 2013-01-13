#include "MaterialCompiler/MaterialCompiler.h"

#include "libxml.h"
#include "libxml/parser.h"

#include <vector>
#include <map>

#include <algorithm>

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
#if defined( CARBON_PLATFORM_WIN32 )
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
#endif
    return true;
}

unsigned int HashString( const char * str )
{
    // FNV hash
    // http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
    // Thx !
    //
    unsigned int h = 2166136261;

    char c;
    while ( (c = *(++str)) != 0 ) // be sure that the string ends by '\0'
    {
        h = ( h * 16777619 ) ^ c;
    }
    return h;
};

const char samplersFileName[] = "samplers";

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
        unsigned char min   : 1;
        unsigned char mag   : 1;
        unsigned char mip   : 2;
        unsigned char wrap  : 2;
    } d;
};

std::vector< Sampler > samplers;

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
            else
            {
                ++a;
            }

            ++a;
        }

        samplers.push_back( sampler );
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
    filename += ".xml";

    int success = xmlSAXUserParseFile( &SAXHandler, NULL, filename.c_str() );

    xmlCleanupParser();
    xmlMemoryDump();
    
    return success == 0;
}

enum ParseState
{
    START,
        DESCRIPTION,
            PARAMETER,
        SETS,
            SET,
                VALUE,
    FINISH
};

struct Texture
{
    char            id[32];
    char            sampler[32];
    unsigned int    layout;
    char            default[256];
};

struct Parameter
{
    char            id[32];
    unsigned int    layout;
    char            desc[256];
};

struct Description
{
    std::vector< Texture >      textures;
    std::vector< Parameter >    parameters;
};

struct Value
{
    char    ref[32];
    char    data[256];
};

struct Set
{
    char                    id[32];
    float                   diffuse[4];
    float                   specular[4];
    float                   ambient[4];
    float                   emission[4];
    float                   shininess;
    std::vector< Value >    values;
};

struct Program
{
    char                id[32];
    Description         desc;
    std::vector< Set >  sets;
};

std::vector< Program > programs;

void startElementPrograms( void * ctx, const xmlChar * name, const xmlChar ** atts )
{
    ParseState& state = *reinterpret_cast< ParseState * >( ctx );

    switch ( state )
    {
    case START :
        if ( strcmp( (const char*)name, "description" ) == 0 )
        {
            Set s;
            s.id[0] = 0;
            memset( s.diffuse, 0, sizeof(s.diffuse) );
            memset( s.specular, 0, sizeof(s.diffuse) );
            memset( s.ambient, 0, sizeof(s.diffuse) );
            memset( s.emission, 0, sizeof(s.diffuse) );
            s.shininess = 0.0f;

            if ( atts )
            {
                const xmlChar ** a = atts;
                while ( *a != NULL )
                {
                    if ( strcmp( (const char *)*a, "diffuse" ) == 0 )
                    {
                        sscanf( (const char *)*(++a), "%f %f %f %f", s.diffuse, s.diffuse+1, s.diffuse+2, s.diffuse+3 );
                    }
                    else if ( strcmp( (const char *)*a, "specular" ) == 0 )
                    {
                        sscanf( (const char *)*(++a), "%f %f %f %f", s.specular, s.specular+1, s.specular+2, s.specular+3 );
                    }
                    else if ( strcmp( (const char *)*a, "ambient" ) == 0 )
                    {
                        sscanf( (const char *)*(++a), "%f %f %f %f", s.ambient, s.ambient+1, s.ambient+2, s.ambient+3 );
                    }
                    else if ( strcmp( (const char *)*a, "emission" ) == 0 )
                    {
                        sscanf( (const char *)*(++a), "%f %f %f %f", s.emission, s.emission+1, s.emission+2, s.emission+3 );
                    }
                    else if ( strcmp( (const char *)*a, "shininess" ) == 0 )
                    {
                        sscanf( (const char *)*(++a), "%f", &s.shininess );
                    }
                    else
                    {
                        ++a;
                    }

                    ++a;
                }
            }

            programs.back().sets.push_back( s );

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
            Texture t;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "id" ) == 0 )
                {
                    strcpy( t.id, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "sampler" ) == 0 )
                {
                    strcpy( t.sampler, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "layout" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%d", &t.layout );
                }
                else if ( strcmp( (const char *)*a, "default" ) == 0 )
                {
                    strcpy( t.default, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }

                ++a;
            }

            programs.back().desc.textures.push_back( t );
        }
        else if ( strcmp( (const char*)name, "parameter" ) == 0 )
        {
            Parameter p;
            p.desc[0] = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "id" ) == 0 )
                {
                    strcpy( p.id, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "layout" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%d", &p.layout );
                }
                else if ( strcmp( (const char *)*a, "desc" ) == 0 )
                {
                    strcpy( p.desc, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }

                ++a;
            }

            programs.back().desc.parameters.push_back( p );

            state = PARAMETER;
        }
        break;
    case SETS :
        if ( strcmp( (const char*)name, "set" ) == 0 )
        {
            Set s;
            memset( s.diffuse, 0, sizeof(s.diffuse) );
            memset( s.specular, 0, sizeof(s.diffuse) );
            memset( s.ambient, 0, sizeof(s.diffuse) );
            memset( s.emission, 0, sizeof(s.diffuse) );
            s.shininess = 0.0f;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "id" ) == 0 )
                {
                    strcpy( s.id, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "diffuse" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%f %f %f %f", s.diffuse, s.diffuse+1, s.diffuse+2, s.diffuse+3 );
                }
                else if ( strcmp( (const char *)*a, "specular" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%f %f %f %f", s.specular, s.specular+1, s.specular+2, s.specular+3 );
                }
                else if ( strcmp( (const char *)*a, "ambient" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%f %f %f %f", s.ambient, s.ambient+1, s.ambient+2, s.ambient+3 );
                }
                else if ( strcmp( (const char *)*a, "emission" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%f %f %f %f", s.emission, s.emission+1, s.emission+2, s.emission+3 );
                }
                else if ( strcmp( (const char *)*a, "shininess" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%f", &s.shininess );
                }
                else
                {
                    ++a;
                }

                ++a;
            }

            programs.back().sets.push_back( s );

            state = SET;
        }
        break;
    case SET :
        if ( strcmp( (const char*)name, "value" ) == 0 )
        {
            Value v;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "ref" ) == 0 )
                {
                    strcpy( v.ref, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }

                ++a;
            }

            programs.back().sets.back().values.push_back( v );

            state = VALUE;
        }
        break;
    case FINISH :
        if ( strcmp( (const char*)name, "program" ) == 0 )
        {
            programs.push_back( Program() );

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
    case PARAMETER :
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
    case PARAMETER :
        if (  strcmp( (const char *)name, "parameter" ) == 0 )
        {
            const Parameter& p = programs.back().desc.parameters.back();

            Value v;
            strcpy( v.ref, p.id );
            strcpy( v.data, programs_characters.c_str() );
            programs_characters.clear();

            programs.back().sets.back().values.push_back( v );

            state = DESCRIPTION;
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
            strcpy( programs.back().sets.back().values.back().data, programs_characters.c_str() );
            programs_characters.clear();

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
    if ( !FindFiles( search_str.c_str(), filenames, false ) )
        return false;

    std::vector< std::string >::const_iterator it = filenames.begin();
    std::vector< std::string >::const_iterator end = filenames.end();
    for ( ; it != end; ++it )
    {
        std::string name = it->substr( 0, it->length() - 4 );

        if ( strcmp( samplersFileName, name.c_str() ) == 0 )
            continue;

        std::string filename = inDir;
        filename += *it;

        if ( xmlSAXUserParseFile( &SAXHandler, &state, filename.c_str() ) != 0 )
            break;

        strcpy( programs.back().id, name.c_str() );
    }

    xmlCleanupParser();
    xmlMemoryDump();
    
    return it == filenames.end();
}

bool BuildMaterials( const char * outDir )
{
    // Compile sampler list
    {
        std::string filename = outDir;
        filename += samplersFileName;
        filename += ".bin";

        if ( !BuildDirectory( filename.c_str() ) )
        {
            return false;
        }

        FILE *fp;

        if (fopen_s(&fp, filename.c_str(), "wb"))
        {
            return false;
        }

        unsigned int sampler_count = samplers.size();

        fwrite(&sampler_count,1,4,fp);

        std::vector< Sampler >::const_iterator samplerIt    = samplers.cbegin();
        std::vector< Sampler >::const_iterator samplerItEnd = samplers.cend();
        for ( ; samplerIt != samplerItEnd; ++samplerIt )
        {
            fwrite( &samplerIt->d, 1, sizeof(Sampler::data), fp );
        }

        fclose(fp);
    }

    // Compile program list
    std::vector< Program >::const_iterator programIt    = programs.cbegin();
    std::vector< Program >::const_iterator programItEnd = programs.cend();
    for ( ; programIt != programItEnd; ++programIt )
    {
        const Program& prg = *programIt;

        std::string filename = outDir;
        filename += prg.id;
        filename += ".bin";

        if ( !BuildDirectory( filename.c_str() ) )
        {
            return false;
        }

        FILE *fp;

        if (fopen_s(&fp, filename.c_str(), "wb"))
        {
            return false;
        }

        unsigned int textureCount = prg.desc.textures.size();

        fwrite( &textureCount, 1, 4, fp );

        std::vector< Texture >::const_iterator textureIt    = prg.desc.textures.cbegin();
        std::vector< Texture >::const_iterator textureItEnd = prg.desc.textures.cend();
        for ( ; textureIt != textureItEnd; ++textureIt )
        {
            unsigned int i=0;
            for ( ; i<samplers.size(); ++i )
            {
                if ( strcmp( textureIt->sampler, samplers[i].id ) == 0 )
                {
                    fwrite( &i, 1, 4, fp );
                    break;
                }
            }

            if ( i == samplers.size() )
            {
                fclose(fp);
                return false;
            }
        }

        unsigned int setSize = prg.desc.parameters.size() * sizeof(float[4]);

        fwrite( &setSize, 1, 4, fp );

        unsigned int setCount = prg.sets.size();
        
        fwrite( &setCount, 1, 4, fp );

        std::vector< Set >::const_iterator setIt    = prg.sets.cbegin();
        std::vector< Set >::const_iterator setItEnd = prg.sets.cend();
        for ( ; setIt != setItEnd; ++setIt )
        {
            unsigned int id = HashString( setIt->id );

            fwrite( &id, 1, 4, fp );

            float uniform_buffer[512];
            memset(uniform_buffer,0,setSize);

            std::vector< Value >::const_iterator valueIt    = setIt->values.cbegin();
            std::vector< Value >::const_iterator valueItEnd = setIt->values.cend();
            for ( ; valueIt != valueItEnd; ++valueIt )
            {
                for ( unsigned int i=0; i<prg.desc.parameters.size(); ++i )
                {
                    if ( strcmp( valueIt->ref, prg.desc.parameters[i].id ) == 0 )
                    {
                        float * o = uniform_buffer + prg.desc.parameters[i].layout * 4;
                        sscanf( valueIt->data, "%f %f %f %f", o, o+1, o+2, o+3 );
                        break;
                    }
                }
            }

            fwrite( &uniform_buffer, 1, setSize, fp );
        }

        fclose(fp);
    }

    return true;
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
