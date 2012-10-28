#include <cstdlib>
#include <cstdio>

#include "UnitTest/Level0/Level0.h"
#include "UnitTest/Level1/Level1.h"
#include "UnitTest/Level2/Level2.h"

int main( int argc, char* argv[] )
{
    //Level0();
    //Level1();
    Level2();

    printf("Press 'Enter' to continue!\n");
    getchar();

    return EXIT_SUCCESS;
}
