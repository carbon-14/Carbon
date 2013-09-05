#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "BSPConverter/BSPConverter.h"

int main( int argc, char* argv[] )
{
    if ( argc < 3 || argc > 4 )
        goto exit;

    char * oFile = argv[--argc];
    char * iFile = argv[--argc];

    int tesselationLevel = 2;
    if ( argc == 2 )
    {
        sscanf( argv[--argc], "%i", &tesselationLevel );
    }

    if ( ! ConvertBSP( iFile, oFile, tesselationLevel ) )
    {
        printf( "CONVERSION FAILED !\n" );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

exit :
    printf( "usage: BSPConverter <teselation_lvl> <input> <output>\n\n" );
    printf( "    input      tesselation level ( default : 2 )       ( optional )\n" );
    printf( "    input      input file name                         ( required )\n" );
    printf( "    output     output file name                        ( required )\n" );
    return 0;
}
