#include "Core/Resource.h"

#include "Core/Hash.h"
#include "Core/ResourceManager.h"

namespace Core
{
    Resource::Resource( const Char * name )
        : m_state( INVALID )
        , m_hashCode( HashString( name ) )
#if defined( CARBON_DEBUG )
        , m_name( name )
#endif
    {
    }

    Bool Resource::IsValid() const
    {
        return m_state != INVALID;
    }

    Bool Resource::IsLoaded() const
    {
        return m_state == LOADED;
    }

    U32 Resource::GetHashCode() const
    {
        return m_hashCode;
    }

#if defined( CARBON_DEBUG )
    const Char * Resource::GetName() const
    {
        return m_name.ConstPtr();
    }
#endif

    void Resource::SelfDelete()
    {
        ResourceManager::Destroy( this );
    }
}
