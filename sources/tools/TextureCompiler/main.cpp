#include <cstdlib>
#include <cstdio>

#include "TextureCompiler/TextureCompiler.h"

int main( int argc, char* argv[] )
{
    if ( argc < 4 || argc > 6 )
        goto exit;

    char * inFile = argv[--argc];
    char * outFile = argv[--argc];

    int options = 0;
    while ( argc > 2 )
    {
        char * option = argv[--argc];
        if ( strcmp( option, "-cmp" ) == 0 )
            options |= OT_COMPRESS;
        else if ( strcmp( option, "-mipmap" ) == 0 )
            options |= OT_MIPMAPGEN;
        else
            goto exit;
    }

    char * profileName = argv[--argc];

    TextureProfile profile;
    if ( strcmp( profileName, "color" ) == 0 )
        profile = TP_COLOR;
    else if ( strcmp( profileName, "linear" ) == 0 )
        profile = TP_LINEAR;
    else if ( strcmp( profileName, "normal" ) == 0 )
        profile = TP_NORMAL;
    else
        goto exit;

    if ( ! CompileTexture( inFile, outFile, profile, options ) )
    {
        printf( "COMPILATION FAILED !\n" );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

exit :
    printf( "usage: TextureCompiler [profile] [-cmp|-mipmap] <output> <input>\n\n" );
    printf( "   profile     color, linear, normal       ( required )\n" );
    printf( "   cmp         compress texture            ( optional )\n" );
    printf( "   mipmap      generate mip map levels     ( optional )\n" );
    printf( "   output      output file name            ( required )\n" );
    printf( "   input       input file name             ( required )\n" );
    return 0;
}
