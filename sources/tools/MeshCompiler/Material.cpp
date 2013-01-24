#include "MeshCompiler/Material.h"

#include "libxml.h"
#include "libxml/parser.h"

#include <algorithm>

enum ParseState
{
    START,
    FINISH
};

struct ParseParam
{
    Program *   program;
    ParseState  state;
};

void startElementProgram( void * ctx, const xmlChar * name, const xmlChar ** atts )
{
    ParseParam& param = *reinterpret_cast< ParseParam * >( ctx );

    switch ( param.state )
    {
    case START :
        if ( strcmp( (const char*)name, "texture" ) == 0 )
        {
            Texture t;
            *t.semantic = 0;

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "id" ) == 0 )
                {
                    strcpy( t.id, (const char *)*(++a) );
                }
                else if ( strcmp( (const char *)*a, "layout" ) == 0 )
                {
                    sscanf( (const char *)*(++a), "%d", &t.layout );
                }
                else if ( strcmp( (const char *)*a, "semantic" ) == 0 )
                {
                    strcpy( t.semantic, (const char *)*(++a) );
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

            param.program->textures.push_back( t );
        }
        else if ( strcmp( (const char*)name, "set" ) == 0 )
        {
            char id[32];

            const xmlChar ** a = atts;
            while ( *a != NULL )
            {
                if ( strcmp( (const char *)*a, "id" ) == 0 )
                {
                    strcpy( id, (const char *)*(++a) );
                }
                else
                {
                    ++a;
                }

                ++a;
            }

            param.program->sets.push_back( id );
        }
        break;
    case FINISH :
        if ( strcmp( (const char*)name, "program" ) == 0 )
        {
            param.state = START;
        }
        break;
    }
}

void endElementProgram( void * ctx, const xmlChar * name )
{
    ParseParam& param = *reinterpret_cast< ParseParam * >( ctx );

    switch ( param.state )
    {
    case START :
        if ( strcmp( (const char*)name, "program" ) == 0 )
        {
            param.state = FINISH;
        }
        break;
    case FINISH :
        break;
    }
}

bool FillProgram( Program& prg, const char * dir )
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
        &startElementProgram,   //startElementSAXFunc startElement;
        &endElementProgram,     //endElementSAXFunc endElement;
        NULL,                   //referenceSAXFunc reference;
        NULL,                   //charactersSAXFunc characters;
        NULL,                   //ignorableWhitespaceSAXFunc ignorableWhitespace;
        NULL,                   //processingInstructionSAXFunc processingInstruction;
        NULL,                   //commentSAXFunc comment;
        NULL,                   //warningSAXFunc warning;
        NULL,                   //errorSAXFunc error;
        NULL                    //fatalErrorSAXFunc fatalError;
    };


    ParseParam param;
    param.program = &prg;
    param.state = FINISH;

    char fileName[256];
    sprintf( fileName, "%s/%s/%s.xml", dir, materialDataDir, prg.id );

    if ( xmlSAXUserParseFile( &SAXHandler, &param, fileName ) != 0 )
        return false;

    xmlCleanupParser();
    xmlMemoryDump();

    std::sort( prg.textures.begin(), prg.textures.end(), CmpByLayout< Texture > );
    
    return true;
}
