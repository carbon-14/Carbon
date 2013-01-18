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
    ResourceRequestStack resourceRequests[2];
    SizeT resourceRequestIndex = 0;
    ResourceRequestStack * currentResourceRequests = 0;

    //====================================================================================

    void ResourceManager::Initialize()
    {
        CARBON_ASSERT( resourceTable.Count() == 0 );
        CARBON_ASSERT( resourceRequests[0].Empty() );
        CARBON_ASSERT( resourceRequests[1].Empty() );
        CARBON_ASSERT( currentResourceRequests == 0 );

        resourceRequestIndex = 0;
        currentResourceRequests = &resourceRequests[resourceRequestIndex];
    }

    void ResourceManager::Destroy()
    {
        ProcessRequests();

        currentResourceRequests = 0;

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
        currentResourceRequests->PushBack( req );

        resourceTable.Insert( res->GetId(), res );
    }

    void ResourceManager::Remove( Resource * res )
    {
        ResourceRequest req = { res, PathString() };
        currentResourceRequests->PushBack( req );
    }

    void ResourceManager::ProcessRequests()
    {
        resourceRequestIndex = ( resourceRequestIndex + 1 ) % 2;
        ResourceRequestStack * nextResourceRequests = &resourceRequests[resourceRequestIndex];

        // Proceed resource loading
        while ( ! currentResourceRequests->Empty() )
        {
            ResourceRequest req = currentResourceRequests->Back();
            currentResourceRequests->PopBack();

            if ( req.m_res->GetRefCount() == 0 )
            {
                nextResourceRequests->PushBack( req );
            }
            else if ( req.m_res->GetState() == Resource::CREATED )
            {
                void * data = 0;
                SizeT size;

                FileSystem::Load( req.m_path, data, size );
                req.m_res->Load( data );
                UnknownAllocator::Deallocate( data );
            }
        }

        currentResourceRequests->Reserve( 0 );
        currentResourceRequests = nextResourceRequests;

        // Delete unreferenced resources
        while ( ! currentResourceRequests->Empty() )
        {
            Resource * res = currentResourceRequests->Back().m_res;
            currentResourceRequests->PopBack();

            if ( res->GetRefCount() == 0 )
            {
                resourceTable.Remove( res->GetId() );
                res->~Resource();
                UnknownAllocator::Deallocate( res );
            }
        }
    }
}
