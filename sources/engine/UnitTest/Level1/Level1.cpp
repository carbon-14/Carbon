#include "Level1.h"

#include "Core/MemoryManager.h"
#include "Core/Array.h"

#include "Core/Trace.h"
#include "Core/Timer.h"
#include "Core/StringUtils.h"

#define DEBUG_STRING_SIZE 128

#define ALLOC_TYPE  char
#define ALLOC_COUNT 5000000

void * allocs[ ALLOC_COUNT ];

void Test_MemoryManager()
{
    {
        CARBON_AUTO_TIMER( allocID, "Default allocator" );

        for ( int i=0; i<ALLOC_COUNT; ++i )
        {
            allocs[ i ] = (U8*)Core::MemoryManager::Malloc( sizeof( ALLOC_TYPE ) );
        }
    }

    for ( int i=0; i<ALLOC_COUNT; ++i )
    {
        Core::MemoryManager::Free( allocs[ i ] );
    }

    {
        CARBON_AUTO_TIMER( allocID, "Frame allocator" );

        for ( int i=0; i<ALLOC_COUNT; ++i )
        {
            allocs[ i ] = Core::MemoryManager::FrameAlloc( sizeof( ALLOC_TYPE ) );
        }
    }
}

void Test_Array()
{
    Array< U32 > a;
    Array< U32 > b( 16 );
    StaticArray< U32, 21 > c( 21, 42 );
    StaticArray< U32, 16 > d( 12 );

    a = b;

    Char debugString[ DEBUG_STRING_SIZE ];
    
    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "taille de b : %d\n", b.Size() ) );

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "capacite de b : %d\n", b.Capacity() ) );

    const U32 * read_data = b.ReadData();
    read_data += 7;

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "element 7 de b : %d\n", *read_data ) );
    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "element 7 de b : %d\n", b.At( 7 ) ) );
    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "element 7 de b : %d\n", b[ 7 ] ) );

    U32 * write_data = b.GetData();
    write_data += 7;
    *write_data = 27;

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "modification de l'element 7 de b : %d\n", *write_data ) );

    b.At( 7 ) = 28;

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "modification de l'element 7 de b : %d\n", b.At( 7 ) ) );

    b[ 7 ] = 29;

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "modification de l'element 7 de b : %d\n", b[ 7 ] ) );

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "front de b : %d\n", b.Front() ) );

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "back de b : %d\n", b.Back() ) );

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "a est-il vide ? : %s\n", a.Empty() ? "oui" : "non" ) );

    a.Reserve( 27 );

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "a.Reserve( 4 ), capacite : %d; taille : %d\n", a.Capacity(), a.Size() ) );

    a.Resize( 10 );

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "a.Resize( 10 ), capacite : %d; taille : %d\n", a.Capacity(), a.Size() ) );

    a.PushBackEmpty();

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "push back empty sur a, back de a : %d; taille : %d\n", a.Back(), a.Size() ) );

    a.PushBack( 45 );

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "push back 45 sur a, back de a : %d; taille : %d\n", a.Back(), a.Size() ) );

    a.PushBack( c.Begin(), c.Begin() + 4 );

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "push back des 4 premiers elements de b sur a, back de a : %d; taille : %d\n", a.Back(), a.Size() ) );

    a.PopBack();

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "pop back sur a, back de a : %d; taille : %d\n", a.Back(), a.Size() ) );

    a.Clear();

    CARBON_TRACE( StringUtils::FormatString( debugString, DEBUG_STRING_SIZE, "clear de a, taille : %d\n", a.Size() ) );
}

void Level1()
{
    Core::MemoryManager::Initialize( sizeof( ALLOC_TYPE ) * ( ALLOC_COUNT + 1 ) );

    Test_MemoryManager();
    Test_Array();

    Core::MemoryManager::Finish();
}
