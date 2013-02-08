#pragma once
#ifndef _CORE_IARRAY_H
#define _CORE_IARRAY_H

#include "Core/DLL.h"
#include "Core/Types.h"

//====================================================================================
// IArray
//====================================================================================

template < typename T >
class IArray
{
public:
    typedef T               ValueType;
    typedef T *             Pointer;
    typedef const T *       ConstPointer;
    typedef T&              Reference;
    typedef const T&        ConstReference;
    typedef SizeT           SizeType;
    typedef T *             Iterator;
    typedef const T *       ConstIterator;

public:
    virtual ConstPointer    ConstPtr() const = 0;
    virtual Pointer         Ptr() = 0;

    virtual ConstIterator   Begin() const = 0;
    virtual Iterator        Begin() = 0;

    virtual ConstIterator   End() const = 0;
    virtual Iterator        End() = 0;

    virtual ConstReference  Front() const = 0;
    virtual Reference       Front() = 0;

    virtual ConstReference  Back() const = 0;
    virtual Reference       Back() = 0;

    virtual ConstReference  At( SizeType index ) const = 0;
    virtual Reference       At( SizeType index ) = 0;

    virtual ConstReference  operator[]( SizeType index ) const = 0;
    virtual Reference       operator[]( SizeType index ) = 0;

    virtual Bool            Empty() const = 0;
    virtual SizeType        Size() const = 0;
    virtual SizeType        Capacity() const = 0;

    virtual void            Resize( SizeType size, ConstReference value = ValueType() ) = 0;

    virtual void            PushBack( ConstReference value ) = 0;
    virtual void            PushBack( ConstPointer p, SizeType n ) = 0;
    virtual void            PushBack( ConstIterator begin, ConstIterator end ) = 0;

    virtual void            PopBack() = 0;
    virtual void            Clear() = 0;
};

//======================================================================== IArray

#endif // _CORE_IARRAY_H
