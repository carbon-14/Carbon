#pragma once
#ifndef _CORE_ARRAY_H
#define _CORE_ARRAY_H

#include "Core/IArray.h"

#include "Core/MemoryManager.h"
#include "Core/MemoryUtils.h"
#include "Core/Algorithm.h"

#include "Core/Assert.h"

namespace Core
{
    //====================================================================================
    // Array
    //====================================================================================

    template < typename T, typename Alloc = DefaultAllocator >
    class Array : public IArray< T >
    {
    public:
        typedef T                   ValueType;
        typedef T *                 Pointer;
        typedef const T *           ConstPointer;
        typedef T&                  Reference;
        typedef const T&            ConstReference;
        typedef SizeT               SizeType;
        typedef T *                 Iterator;
        typedef const T *           ConstIterator;

    public:
        Array();
        Array( SizeType size, ConstReference value = ValueType() );
        Array( ConstIterator v, SizeType n );
        Array( ConstIterator begin, ConstIterator end );
        ~Array();

        ConstPointer    ConstPtr() const;
        Pointer         Ptr();

        ConstIterator   Begin() const;
        Iterator        Begin();

        ConstIterator   End() const;
        Iterator        End();

        ConstReference  Front() const;
        Reference       Front();

        ConstReference  Back() const;
        Reference       Back();

        ConstReference  At( SizeType index ) const;
        Reference       At( SizeType index );

        ConstReference  operator[]( SizeType index ) const;
        Reference       operator[]( SizeType index );

        Bool            Empty() const;
        SizeType        Size() const;
        SizeType        Capacity() const;

        void            Reserve( SizeType capacity );
        void            Resize( SizeType size, ConstReference value = ValueType() );

        void            PushBackEmpty();
        void            PushBack( ConstReference value );
        void            PushBack( ConstPointer p, SizeType n );
        void            PushBack( ConstIterator begin, ConstIterator end );

        void            PopBack();
        void            Clear();

    protected:
        Pointer         DoAllocate( SizeType capacity );
        void            DoFree( Pointer memory );
        SizeType        ComputeNewCapacity() const;
        void            IncreaseCapacity( SizeType capacity );

    protected:
        ValueType *	    m_begin;
        ValueType *	    m_end;
        ValueType *	    m_capacity;

    private:
        // forbidden
        Array( const Array& other );
        Array&          operator=( const Array& other );
    };

    //============================================================================== Array

    template< typename T, typename Alloc >
    Array< T, Alloc >::Array()
    : m_begin( 0 ), m_end( 0 ), m_capacity( 0 )
    {
    }

    template< typename T, typename Alloc >
    Array< T, Alloc >::Array( SizeType size, ConstReference value = ValueType() )
    {
        if ( size > 0 )
        {
            m_begin	= DoAllocate( size );
            m_capacity = m_end = m_begin + size;
            Fill( m_begin, m_end, value );
        }
        else
        {
            m_begin = m_end = m_capacity = 0;
        }
    }

    template< typename T, typename Alloc >
    Array< T, Alloc >::Array( ConstIterator v, SizeType n )
    {
        if ( n > 0 )
        {
            m_begin	= DoAllocate( n );
            m_capacity = m_end = m_begin + n;
            Copy( v, v+n, m_begin );
        }
        else
        {
            m_begin = m_end = m_capacity = 0;
        }
    }

    template< typename T, typename Alloc >
    Array< T, Alloc >::Array( ConstIterator begin, ConstIterator end )
    {
        const SizeType size = static_cast< SizeType >( end - begin );
        if ( size > 0 )
        {
            m_begin	= DoAllocate( size );
            m_capacity = m_end = m_begin + size;
            Copy( begin, end, m_begin );
        }
        else
        {
            m_begin = m_end = m_capacity = 0;
        }
    }

    template< typename T, typename Alloc >
    Array< T, Alloc >::~Array()
    {
        Clear();
        DoFree( m_begin );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::ConstPointer Array< T, Alloc >::ConstPtr() const
    {
        return m_begin;
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::Pointer Array< T, Alloc >::Ptr()
    {
        return m_begin;
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::ConstIterator Array< T, Alloc >::Begin() const
    {
        return m_begin;
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::Iterator Array< T, Alloc >::Begin()
    {
        return m_begin;
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::ConstIterator Array< T, Alloc >::End() const
    {
        return m_end;
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::Iterator Array< T, Alloc >::End()
    {
        return m_end;
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::ConstReference Array< T, Alloc >::Front() const
    {
        CARBON_ASSERT( m_begin );
        return *m_begin;
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::Reference Array< T, Alloc >::Front()
    {
        CARBON_ASSERT( m_begin );
        return *m_begin;
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::ConstReference Array< T, Alloc >::Back() const
    {
        CARBON_ASSERT( m_begin );
        return *( m_end - 1 );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::Reference Array< T, Alloc >::Back()
    {
        CARBON_ASSERT( m_begin );
        return *( m_end - 1 );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::ConstReference Array< T, Alloc >::At( SizeType index ) const
    {
        CARBON_ASSERT( index < Size() );
        return *( m_begin + index );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::Reference Array< T, Alloc >::At( SizeType index )
    {
        CARBON_ASSERT( index < Size() );
        return *( m_begin + index );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::ConstReference Array< T, Alloc >::operator[]( SizeType index ) const
    {
        return At( index );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::Reference Array< T, Alloc >::operator[]( SizeType index )
    {
        return At( index );
    }

    template< typename T, typename Alloc >
    Bool Array< T, Alloc >::Empty() const
    {
        return ( m_begin == m_end );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::SizeType Array< T, Alloc >::Size() const
    {
        return static_cast< SizeType >( m_end - m_begin );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::SizeType Array< T, Alloc >::Capacity() const
    {
        return static_cast< SizeType >( m_capacity - m_begin );
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::Reserve( SizeType capacity )
    {
        if ( Capacity() < capacity )
        {
            IncreaseCapacity( capacity );
        }
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::Resize( SizeType size, ConstReference value )
    {
        Iterator end = m_begin + size;
        if ( size < Size() )
        {
            while ( m_end != end )
            {
                PopBack();
            }
        }
        else
        {
            Reserve( size );
            Fill( m_end, m_capacity, value );
            m_end = m_capacity;
        }
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::PushBackEmpty()
    {
        if ( m_end == m_capacity )
        {
            IncreaseCapacity( ComputeNewCapacity() );
        }
        *m_end = ValueType();
        ++m_end;
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::PushBack( ConstReference value )
    {
        if ( m_end == m_capacity )
        {
            IncreaseCapacity( ComputeNewCapacity() );
        }
        *m_end = value;
        ++m_end;
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::PushBack( ConstPointer p, SizeType n )
    {
        PushBack( p, p + n );
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::PushBack( ConstIterator begin, ConstIterator end )
    {
        const SizeType capacity = Size() + ( end - begin );
        if ( ( m_begin + capacity ) > m_capacity )
        {
            IncreaseCapacity( capacity );
        }
        Copy( begin, end, m_end );
        m_end = m_begin + capacity;
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::PopBack()
    {
        CARBON_ASSERT( m_begin < m_end );
        --m_end;
        m_end->~ValueType();
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::Clear()
    {
        while ( m_begin != m_end )
        {
            PopBack();
        }
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::Pointer Array< T, Alloc >::DoAllocate( SizeType capacity )
    {
        return static_cast< Pointer >( Alloc::Malloc( capacity * sizeof( ValueType ), MemoryUtils::AlignOf< ValueType >() ) );
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::DoFree( Pointer memory )
    {
        Alloc::Free( memory );
    }

    template< typename T, typename Alloc >
    typename Array< T, Alloc >::SizeType Array< T, Alloc >::ComputeNewCapacity() const
    {
        return ( m_capacity == 0 ) ? 1 : 2 * Capacity();
    }

    template< typename T, typename Alloc >
    void Array< T, Alloc >::IncreaseCapacity( SizeType capacity )
    {
        Pointer ptr = DoAllocate( capacity );
        Copy( m_begin, m_end, ptr );
        DoFree( m_begin );
        m_end = ptr + Size();
        m_begin = ptr;
        m_capacity = ptr + capacity;
    }

    //============================================================================== Array
}

#endif // _CORE_ARRAY_H