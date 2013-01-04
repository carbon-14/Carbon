#include "Core/Resource.h"

#include "Core/Hash.h"
#include "Core/ResourceManager.h"

namespace Core
{
    Resource::Resource()
        : m_state( CREATED )
    {
    }

    Resource::~Resource()
    {
    }

    Resource::State Resource::GetState() const
    {
        return m_state;
    }

    Bool Resource::IsValid() const
    {
        return m_state != INVALID;
    }

    Bool Resource::IsLoaded() const
    {
        return m_state == LOADED;
    }

    U32 Resource::GetId() const
    {
        return m_id;
    }

#if defined( CARBON_DEBUG )
    const Char * Resource::GetName() const
    {
        return m_name.ConstPtr();
    }
#endif

    U32 Resource::MakeIdFromName( const Char * name )
    {
        return HashString( name );
    }

    void Resource::SetId( U32 id )
    {
        m_id = id;
    }

#if defined( CARBON_DEBUG )
    void Resource::SetName( const Char * name )
    {
        m_name = name;
    }
#endif

    void Resource::SelfDelete()
    {
        ResourceManager::Destroy( this );
    }
}
