#include "UnitTest/Level0/Level0.h"
#include "UnitTest/Utils.h"

#include "Core/Platform.h"

using namespace Core;

void Level0()
{
    UNIT_TEST_MESSAGE( "\n###########\n# LEVEL 0 #\n###########\n\n" )

    UNIT_TEST_MESSAGE( "Hello World !!!\n" )
    UNIT_TEST_MESSAGE( "Your platform is : %s\n", Platform::GetName() )
}
