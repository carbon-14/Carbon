#pragma once
#ifndef _CORE_RESOURCEMANAGER_H
#define _CORE_RESOURCEMANAGER_H

#include "Core/DLL.h"
#include "Core/Types.h"

namespace Core
{
    class Resource;

    //====================================================================================
    // ResourceManager
    //====================================================================================

    class _CoreExport ResourceManager
    {
    public:
        static void Initialize();
        static void Destroy();

        template < typename T > static T * Create( const Char * name );
        template < typename T > static void Destroy( T * res );

    private:
        static Resource *   Find( const Char * name );
        static void         Add( Resource * res );
        static void         Remove( Resource * res );
    };

    //==================================================================== ResourceManager

    template < typename T >
    T * ResourceManager::Create( const Char * name )
    {
        Resource * res = Find( name );
        if ( !res )
        {
            res = static_cast< Resource * >( UnknownAllocator::Allocate( sizeof(T), MemoryUtils::AlignOf< T >() ) );
            ::new( res ) T( name );

            PathString path;
            FileSystem::BuildPathName( name, path, FileSystem::PT_CACHE );

            void * data = 0;
            SizeT size;
            FileSystem::Load( path, data, size );
            
            res->Load( data );

            UnknownAllocator::Deallocate( data );

            Add( res );
        }

        return static_cast< T * >( res );
    }

    template < typename T >
    void ResourceManager::Destroy( T * res )
    {
        Remove( res );
    }

    //==================================================================== ResourceManager
}

#endif // _CORE_RESOURCEMANAGER_H
