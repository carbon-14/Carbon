#include "MeshCompiler/MeshCompiler.h"

#include "libxml.h"
#include "libxml/parser.h"

bool CompileMesh()
{
    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}
