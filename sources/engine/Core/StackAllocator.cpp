#include "Core/StackAllocator.h"

#include "Core/MemoryUtils.h"
#include "Core/Assert.h"

namespace Core
{
    StackAllocator::StackAllocator()
        : mp_buffer(0), m_end(0), m_head(0)
    {
    }

    void StackAllocator::Initialize( void * buffer, SizeT sizeBytes )
    {
        CARBON_ASSERT( buffer != 0 );
        CARBON_ASSERT( sizeBytes > 0 );
        CARBON_ASSERT( mp_buffer == 0 );

        mp_buffer = buffer;
        m_head = reinterpret_cast< SizeT >( mp_buffer );
        m_end = m_head + sizeBytes;
    }

    void StackAllocator::Destroy()
    {
        mp_buffer = 0;
        m_head = 0;
    }

    void StackAllocator::Clear()
    {
        m_head = reinterpret_cast< SizeT >( mp_buffer );
    }

    void * StackAllocator::Allocate( SizeT sizeBytes, SizeT align )
    {
        SizeT a = MemoryUtils::GetNextAlignedAddress( m_head, align );
        
        m_head = a + sizeBytes;

        CARBON_ASSERT( m_head <= m_end );

        return reinterpret_cast< void * >( a );
    }
}
