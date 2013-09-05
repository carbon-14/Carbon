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

    int tesselationLevel = 4;
    if ( argc == 2 )
    {
        sscanf( argv[--argc], "%i", &tesselationLevel );
        if ( tesselationLevel < 0 )
        {
            tesselationLevel = 0;
        }
    }

    if ( ! ConvertBSP( iFile, oFile, tesselationLevel ) )
    {
        printf( "CONVERSION FAILED !\n" );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

exit :
    printf( "usage: BSPConverter <teselation_lvl> <input> <output>\n\n" );
    printf( "    input      tesselation level ( default : 4 )       ( optional )\n" );
    printf( "    input      input file name                         ( required )\n" );
    printf( "    output     output file name                        ( required )\n" );
    return 0;
}
