#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "MaterialCompiler/MaterialCompiler.h"

int main( int argc, char* argv[] )
{
    if ( argc != 2 )
        goto exit;

    char * root = argv[--argc];

    if ( ! CompileMaterial( root ) )
    {
        printf( "COMPILATION FAILED !\n" );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

exit :
    printf( "usage: MaterialCompiler <root>\n\n" );
    printf( "    root    root directory ( required )\n" );
    return 0;
}
