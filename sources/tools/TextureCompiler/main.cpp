#include <cstdlib>
#include <cstdio>

#include "TextureCompiler/TextureCompiler.h"

int main( int argc, char* argv[] )
{
    if ( argc < 5 || argc > 6 )
        goto exit;

    TextureProfile profile;
    if ( strcmp( argv[1], "-profile" ) )
        goto exit;

    if ( strcmp( argv[2], "raw" ) == 0 )
        profile = TP_RAW;
    else if ( strcmp( argv[2], "color" ) == 0 )
        profile = TP_COLOR;
    else if ( strcmp( argv[2], "linear" ) == 0 )
        profile = TP_LINEAR;
    else if ( strcmp( argv[2], "normal" ) == 0 )
        profile = TP_NORMAL;
    else
        goto exit;

    bool mipMapGen = ( argc == 6 && strcmp( argv[3], "-mipmap" ) == 0 );

    return CompileTexture( argv[5], argv[4], profile, mipMapGen );

exit :
    printf( "usage: TextureCompiler [-profile value] [-mipmap] <output> <input>\n\n" );
    printf( "   profile     raw, color, linear, normal  ( required )\n" );
    printf( "   mipmap      generate mip map levels     ( optional )\n" );
    printf( "   output      output file name            ( required )\n" );
    printf( "   input       input file name             ( required )\n" );
    return 0;
}
