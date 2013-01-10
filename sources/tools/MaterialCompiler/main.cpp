#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "MaterialCompiler/MaterialCompiler.h"

int main( int argc, char* argv[] )
{
    if ( argc != 3 )
        goto exit;

    char * inDir = argv[--argc];
    char * outDir = argv[--argc];

    if ( ! CompileMaterial( inDir, outDir ) )
    {
        printf( "COMPILATION FAILED !\n" );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

exit :
    printf( "usage: MaterialCompiler <output> <input>\n\n" );
    printf( "    output     output dir name ( required )\n" );
    printf( "    input      input dir name  ( required )\n" );
    return 0;
}
