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

        static void             Update();

        static const Resource * FindByName( const Char * name );
        static const Resource * FindById( U32 id );

    private:
        static Resource *       Find( U32 id );
        static void             Add( const Char * name, Resource * res );
        static void             Remove( Resource * res );
        static void             ProcessRequests();
    };

    //==================================================================== ResourceManager

    template < typename T >
    T * ResourceManager::Create( const Char * name )
    {
        U32 id = Resource::MakeIdFromName( name );

        Resource * res = Find( id );
        if ( !res )
        {
            res = static_cast< Resource * >( UnknownAllocator::Allocate( sizeof(T), MemoryUtils::AlignOf< T >() ) );
            ::new( res ) T();

            res->SetId( id );
#if defined( CARBON_DEBUG )
            res->SetName( name );
#endif

            Add( name, res );
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
