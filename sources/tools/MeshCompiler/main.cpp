#include <cstdlib>

#include "MeshCompiler/MeshCompiler.h"

int main( int argc, char* argv[] )
{
    int options = 0;

    return CompileMesh( "../../../data/sibenik.dae", "../../../cache/sibenik.bmh", options );
}
