#include "Core/Resource.h"

#include "Core/Hash.h"
#include "Core/ResourceManager.h"

Resource::Resource()
    : m_state( 0 )
{
}

Resource::~Resource()
{
}

Bool Resource::IsValid() const
{
    return (m_state & VALID) != 0;
}

Bool Resource::IsPending() const
{
    return (m_state & PENDING) != 0;
}

Bool Resource::IsLoaded() const
{
    return (m_state & LOADED) != 0;
}

Bool Resource::IsReady() const
{
    return (m_state & (VALID|LOADED)) == (VALID|LOADED);
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
