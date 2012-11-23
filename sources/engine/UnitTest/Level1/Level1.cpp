#include "UnitTest/Level1/Level1.h"
#include "UnitTest/Utils.h"

#include "Core/MemoryManager.h"
#include "Core/Array.h"
#include "Core/FixedArray.h"
#include "Core/String.h"
#include "Core/FixedString.h"

#include "Core/Timer.h"

using namespace Core;

#define ALLOC_TYPE  char
#define ALLOC_COUNT 500000

namespace Level1_NS
{
    void * allocs[ ALLOC_COUNT ];
}

using namespace Level1_NS;

void Test_MemoryManager()
{
    UNIT_TEST_MESSAGE( "* Allocators Test\n\n" );
    UNIT_TEST_MESSAGE( "Alloc count : %d | Alloc block size : %d\n", ALLOC_COUNT, sizeof( ALLOC_TYPE ) );

    {
        CARBON_AUTO_TIMER( allocID, "Default allocator" );

        for ( int i=0; i<ALLOC_COUNT; ++i )
        {
            allocs[ i ] = (U8*)DefaultAllocator::Allocate( sizeof( ALLOC_TYPE ) );
        }
    }

    for ( int i=0; i<ALLOC_COUNT; ++i )
    {
        DefaultAllocator::Deallocate( allocs[ i ] );
    }

    {
        CARBON_AUTO_TIMER( allocID, "Frame allocator" );

        for ( int i=0; i<ALLOC_COUNT; ++i )
        {
            allocs[ i ] = FrameAllocator::Allocate( sizeof( ALLOC_TYPE ) );
        }
    }
}

void Test_Array()
{
    Array< U32 > a;
    Array< U32 > b( 16 );
    FixedArray< U32, 21 > c( 21, 42 );
    FixedArray< U32, 16 > d( 12 );

    String f = "f";

    UNIT_TEST_MESSAGE( "\n* Array Test\n\n" );
    
    UNIT_TEST_MESSAGE( "taille de b : %d\n", b.Size() );

    UNIT_TEST_MESSAGE( "capacite de b : %d\n", b.Capacity() );

    const U32 * read_data = b.ConstPtr();
    read_data += 7;

    UNIT_TEST_MESSAGE( "element 7 de b : %d\n", *read_data );
    UNIT_TEST_MESSAGE( "element 7 de b : %d\n", b.At( 7 ) );
    UNIT_TEST_MESSAGE( "element 7 de b : %d\n", b[ 7 ] );

    U32 * write_data = b.Ptr();
    write_data += 7;
    *write_data = 27;

    UNIT_TEST_MESSAGE( "modification de l'element 7 de b : %d\n", *write_data );

    b.At( 7 ) = 28;

    UNIT_TEST_MESSAGE( "modification de l'element 7 de b : %d\n", b.At( 7 ) );

    b[ 7 ] = 29;

    UNIT_TEST_MESSAGE( "modification de l'element 7 de b : %d\n", b[ 7 ] );

    UNIT_TEST_MESSAGE( "front de b : %d\n", b.Front() );

    UNIT_TEST_MESSAGE( "back de b : %d\n", b.Back() );

    UNIT_TEST_MESSAGE( "a est-il vide ? : %s\n", a.Empty() ? "oui" : "non" );

    a.Reserve( 27 );

    UNIT_TEST_MESSAGE( "a.Reserve( 4 ), capacite : %d; taille : %d\n", a.Capacity(), a.Size() );

    a.Resize( 10 );

    UNIT_TEST_MESSAGE( "a.Resize( 10 ), capacite : %d; taille : %d\n", a.Capacity(), a.Size() );

    a.PushBack( 45 );

    UNIT_TEST_MESSAGE( "push back 45 sur a, back de a : %d; taille : %d\n", a.Back(), a.Size() );

    a.PushBack( c.Begin(), c.Begin() + 4 );

    UNIT_TEST_MESSAGE( "push back des 4 premiers elements de b sur a, back de a : %d; taille : %d\n", a.Back(), a.Size() );

    a.PopBack();

    UNIT_TEST_MESSAGE( "pop back sur a, back de a : %d; taille : %d\n", a.Back(), a.Size() );

    a.Clear();

    UNIT_TEST_MESSAGE( "clear de a, taille : %d\n", a.Size() );
}

void Level1()
{
    UNIT_TEST_MESSAGE( "\n###########\n# LEVEL 1 #\n###########\n\n" );

    MemoryManager::Initialize( sizeof( ALLOC_TYPE ) * ( ALLOC_COUNT + 1 ) );

    Test_MemoryManager();
    Test_Array();

    MemoryManager::Destroy();
}
