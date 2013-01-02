#include "Core/ResourceManager.h"

#include "Core/Hash.h"
#include "Core/HashTable.h"

#include "Core/Resource.h"

#include "Core/Assert.h"
#include "Core/Trace.h"

namespace Core
{
    typedef HashTable< Resource * > ResourceTable;
    ResourceTable m_resourceTable;

    void ResourceManager::Initialize()
    {

    }

    void ResourceManager::Destroy()
    {
        CARBON_ASSERT( m_resourceTable.Count() == 0 );

#if defined( CARBON_DEBUG )
        if ( m_resourceTable.Count() )
        {
            Array< Resource * > unreleased;
            m_resourceTable.Dump( unreleased );

            Char res_msg[512];

            CARBON_TRACE( "====================================================\n" );
            CARBON_TRACE( "# Some resources are leaking\n\n" );

            Array< Resource * >::Iterator it = unreleased.Begin();
            Array< Resource * >::ConstIterator end = unreleased.End();
            for ( ; it != end; ++it )
            {
                Resource * res = *it;

                StringUtils::FormatString( res_msg, sizeof(res_msg), "# %s\n", res->GetName() );
                CARBON_TRACE( res_msg );
            }

            CARBON_TRACE( "\n====================================================\n" );
        }
#endif
    }

    Resource * ResourceManager::Find( const Char * name )
    {
        U32 key = HashString( name );
        Resource ** res = m_resourceTable.Find( key );
        if ( res )
            return *res;
        else
            return 0;
    }

    void ResourceManager::Add( Resource * res )
    {
        m_resourceTable.Insert( res->GetHashCode(), res );
    }

    void ResourceManager::Remove( Resource * res )
    {
        m_resourceTable.Remove( res->GetHashCode() );
    }
}
