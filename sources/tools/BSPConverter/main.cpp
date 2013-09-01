#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "BSPConverter/BSPConverter.h"

int main( int argc, char* argv[] )
{
    if ( argc < 3 || argc > 7 )
        goto exit;

    char * oFile = argv[--argc];
    char * iFile = argv[--argc];

    if ( ! ConvertBSP( iFile, oFile ) )
    {
        printf( "COMPILATION FAILED !\n" );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

exit :
    printf( "usage: BSPConverter <input> <output>\n\n" );
    printf( "    input      input file name                         ( required )\n" );
    printf( "    output     output file name                        ( required )\n" );
    return 0;
}
