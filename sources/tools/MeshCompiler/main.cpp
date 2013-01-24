#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "MeshCompiler/MeshCompiler.h"

int main( int argc, char* argv[] )
{
    if ( argc < 3 || argc > 7 )
        goto exit;

    char * file = argv[--argc];
    char * root = argv[--argc];

    int options = 0;
    while ( argc > 1 )
    {
        char * option = argv[--argc];
        if ( strcmp( option, "-cmp_pos" ) == 0 )
            options |= CT_POSITION;
        else if ( strcmp( option, "-cmp_vec" ) == 0 )
            options |= CT_VECTOR;
        else if ( strcmp( option, "-cmp_uvs" ) == 0 )
            options |= CT_TEXCOORD;
        else if ( strcmp( option, "-gen_ts" ) == 0 )
            options |= OT_GENERATE_TANGENT_SPACE;
        else
            goto exit;
    }

    if ( ! CompileMesh( file, root, options ) )
    {
        printf( "COMPILATION FAILED !\n" );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

exit :
    printf( "usage: MeshCompiler [-cmp_pos|-cmp_vec|-cmp_uvs] <root> <input>\n\n" );
    printf( "    cmp_pos    compress positions                      ( optional )\n" );
    printf( "    cmp_vec    compress normals, tangents, binormals   ( optional )\n" );
    printf( "    cmp_uvs    compress texture coords                 ( optional )\n" );
    printf( "    root       root directory                          ( required )\n" );
    printf( "    input      input file name                         ( required )\n" );
    return 0;
}
