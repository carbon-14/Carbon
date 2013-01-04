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
        PathString  m_path;
    };

    typedef Array< ResourceRequest, FrameAllocator > ResourceRequestStack;
    ResourceRequestStack resourceRequests;

    //====================================================================================

    void ResourceManager::Initialize()
    {
        CARBON_ASSERT( resourceTable.Count() == 0 );
        CARBON_ASSERT( resourceRequests.Empty() );
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

        ResourceRequest req = { res, path };
        resourceRequests.PushBack( req );

        resourceTable.Insert( res->GetId(), res );
    }

    void ResourceManager::Remove( Resource * res )
    {
        ResourceRequest req = { res, PathString() };
        resourceRequests.PushBack( req );
    }

    void ResourceManager::ProcessRequests()
    {
        Array< Resource *, FrameAllocator > toDelete;

        // Proceed resource loading
        while ( ! resourceRequests.Empty() )
        {
            ResourceRequest& req = resourceRequests.Back();

            Resource * res = req.m_res;

            if ( res->GetRefCount() == 0 )
            {
                toDelete.PushBack( res );
            }
            else if ( res->GetState() == Resource::CREATED )
            {
                void * data = 0;
                SizeT size;

                FileSystem::Load( req.m_path, data, size );
                res->Load( data );
                UnknownAllocator::Deallocate( data );
            }

            resourceRequests.PopBack();
        }

        resourceRequests.Reserve( 0 );

        // Delete unreferenced resources
        while ( ! toDelete.Empty() )
        {
            Resource * res = toDelete.Back();

            if ( res->GetRefCount() == 0 )
            {
                resourceTable.Remove( res->GetId() );
                res->~Resource();
                UnknownAllocator::Deallocate( res );
            }

            toDelete.PopBack();
        }
    }
}
