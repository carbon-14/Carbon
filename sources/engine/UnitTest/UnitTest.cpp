#include <cstdlib>
#include <cstdio>

#include "Core/HelloWorld.h"

int main( int argc, const char* argv[] )
{
	printf( "%s\n", HelloWorld() );
	printf( "Your platform is : %s\n", GetPlatformName() );

	return EXIT_SUCCESS;
}
