#pragma once
#ifndef _CORE_STACKALLOCATOR_H
#define _CORE_STACKALLOCATOR_H

#include "Core/Types.h"
#include "Core/DLL.h"

namespace Core
{
    class _CoreExport StackAllocator
    {
    public:
        StackAllocator();

        void Initialize( SizeT sizeBytes );
        void Finish();

        void Clear();

        void * Allocate( SizeT sizeBytes, SizeT align = 1 );

    private:
        void *  mp_buffer;
        SizeT   m_end;
        SizeT   m_head;
    };
}

#endif // _CORE_STACKALLOCATOR_H
