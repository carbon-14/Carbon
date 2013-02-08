#pragma once
#ifndef _CORE_BUCKET_H
#define _CORE_BUCKET_H

#include "Core/Array.h"

//====================================================================================
// Bucket
//====================================================================================

template< typename T, typename Alloc = DefaultAllocator >
class Bucket : public Array< T, Alloc >
{
public:
    typedef T           ValueType;
    typedef T *         Pointer;
    typedef const T *   ConstPointer;
    typedef T&          Reference;
    typedef const T&    ConstReference;
    typedef SizeT       SizeType;
    typedef T *         Iterator;
    typedef const T *   ConstIterator;

    static const SizeT  ms_MinBucketSize = 4;

public:
    Bucket();

    void Remove( Iterator w );
};

//============================================================================= Bucket

template< typename T, typename Alloc >
Bucket< T, Alloc >::Bucket()
{
    m_end = m_begin = DoAllocate( ms_MinBucketSize );
    m_capacity = m_begin + ms_MinBucketSize;
}

template< typename T, typename Alloc >
void Bucket< T, Alloc >::Remove( Iterator w )
{
    if ( ( w + 1 ) != m_end )
    {
        *w = Back();
    }
        
    PopBack();
}

//============================================================================= Bucket

#endif // _CORE_BUCKET_H
