#include <cstdlib>
#include <cstdio>

#include "Level0\Level0.h"
#include "Level1\Level1.h"

int main( int argc, char* argv[] )
{
    Level0();
    Level1();

    printf("Press 'Enter' to continue!\n");
    getchar();

    return EXIT_SUCCESS;
}
