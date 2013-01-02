#include "Core/SharedPtr.h"

namespace Core
{
    RefCounted::RefCounted()
        : m_refCount( 0 )
    {
    }

    void RefCounted::Increment()
    {
        ++m_refCount;
    }

    void RefCounted::Decrement()
    {
        CARBON_ASSERT( m_refCount > 0 );
        --m_refCount;
        if ( !m_refCount )
        {
            SelfDelete();
        }
    }

    SizeT RefCounted::GetRefCount() const
    {
        return m_refCount;
    }
}
