#include "StackAllocator.h"

#include "MemoryUtils.h"
#include "MemoryManager.h"
#include "Assert.h"

namespace Core
{
    StackAllocator::StackAllocator()
        : mp_buffer(0), m_end(0), m_head(0)
    {
    }

    void StackAllocator::Initialize( SizeT sizeBytes )
    {
        CARBON_ASSERT( sizeBytes > 0 );
        CARBON_ASSERT( mp_buffer == 0 );

        mp_buffer = MemoryManager::Malloc( sizeBytes );
        m_head = reinterpret_cast< SizeT >( mp_buffer );
        m_end = m_head + sizeBytes;
    }

    void StackAllocator::Finish()
    {
        MemoryManager::Free( mp_buffer );
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
