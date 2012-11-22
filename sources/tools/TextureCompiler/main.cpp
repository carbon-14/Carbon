#include <cstdlib>

#include "TextureCompiler/TextureCompiler.h"

int main( int argc, char* argv[] )
{
    if ( argc > 2 )
    {
        return CompileTexture( argv[1], argv[2], CF_BC1 );
    }
}
