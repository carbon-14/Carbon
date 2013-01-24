#include "Core/ResourceManager.h"

#include "Core/Hash.h"
#include "Core/HashTable.h"
#include "Core/FileSystem.h"
#include "Core/Resource.h"

#include "Core/Assert.h"
#include "Core/Trace.h"

namespace Core
{
    typedef HashTable< Resource *, U32 > ResourceTable;
    ResourceTable resourceTable;

    struct ResourceRequest
    {
        Resource *  m_res;
        Char        m_path[256];
    };
    CARBON_DECLARE_POD_TYPE( ResourceRequest );

    typedef Array< ResourceRequest, FrameAllocator > ResourceRequestStack;
    ResourceRequestStack toCreateResources;
    ResourceRequestStack toDeleteResources;

    //====================================================================================

    void ResourceManager::Initialize()
    {
        CARBON_ASSERT( resourceTable.Count() == 0 );
        CARBON_ASSERT( toCreateResources.Empty() );
        CARBON_ASSERT( toDeleteResources.Empty() );
    }

    void ResourceManager::Destroy()
    {
        ProcessRequests();

#if defined( CARBON_DEBUG )
        if ( resourceTable.Count() )
        {
            Array< Resource *, FrameAllocator > unreleased;
            resourceTable.Dump( unreleased );

            Char res_msg[512];

            CARBON_TRACE( "====================================================\n" );
            CARBON_TRACE( "# Some resources are leaking\n\n" );

            Array< Resource * >::Iterator it = unreleased.Begin();
            Array< Resource * >::ConstIterator end = unreleased.End();
            for ( ; it != end; ++it )
            {
                Resource * res = *it;

                StringUtils::FormatString( res_msg, sizeof(res_msg), "# %s | ref count : %d\n", res->GetName(), res->GetRefCount() );
                CARBON_TRACE( res_msg );
            }

            CARBON_TRACE( "\n====================================================\n" );
        }
#endif
        CARBON_ASSERT( resourceTable.Count() == 0 );
    }

    void ResourceManager::Update()
    {
        ProcessRequests();
    }

    const Resource * ResourceManager::FindByName( const Char * name )
    {
        U32 id = Resource::MakeIdFromName( name );
        return Find( id );
    }

    const Resource * ResourceManager::FindById( U32 id )
    {
        return Find( id );
    }

    Resource * ResourceManager::Find( U32 id )
    {
        Resource ** res = resourceTable.Find( id );
        if ( res )
            return *res;
        else
            return 0;
    }

    void ResourceManager::Add( const Char * name, Resource * res )
    {
        PathString path;
        FileSystem::BuildPathName( name, path, FileSystem::PT_CACHE );

        ResourceRequest req;
        req.m_res = res;
        StringUtils::StrCpy( req.m_path, 256, path.ConstPtr() );
        toCreateResources.PushBack( req );

        resourceTable.Insert( res->GetId(), res );
    }

    void ResourceManager::Remove( Resource * res )
    {
        ResourceRequest req = { res, 0 };
        toDeleteResources.PushBack( req );
    }

    void ResourceManager::ProcessRequests()
    {
        // Proceed resource loading
        while ( ! toCreateResources.Empty() )
        {
            ResourceRequest req = toCreateResources.Back();
            toCreateResources.PopBack();

            if ( req.m_res->GetRefCount() == 0 )
            {
                toDeleteResources.PushBack( req );
            }
            else if ( req.m_res->IsValid() )
            {
                void * data = 0;
                SizeT size;

                if ( ! ( FileSystem::Load( req.m_path, data, size ) && req.m_res->Load( data ) ) )
                {
                    req.m_res->m_state &= !Resource::VALID;
                }

                req.m_res->m_state |= Resource::LOADED;

                UnknownAllocator::Deallocate( data );
            }
        }

        toCreateResources.Reserve( 0 );

        // Delete unreferenced resources
        while ( ! toDeleteResources.Empty() )
        {
            Resource * res = toDeleteResources.Back().m_res;
            toDeleteResources.PopBack();

            if ( res->GetRefCount() == 0 )
            {
                resourceTable.Remove( res->GetId() );
                if ( res->IsLoaded() )
                    res->Unload();
                res->~Resource();
                UnknownAllocator::Deallocate( res );
            }
        }

        toDeleteResources.Reserve( 0 );

        CARBON_ASSERT( toCreateResources.Empty() );
    }
}
