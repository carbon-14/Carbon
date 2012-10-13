#pragma once
#ifndef _CORE_FIXEDARRAY_H
#define _CORE_FIXEDARRAY_H

#include "Core/IArray.h"

#include "Core/Algorithm.h"

#include "Core/Assert.h"

namespace Core
{
    //====================================================================================
    // FixedArray
    //====================================================================================

    template < typename T, SizeT C >
    class FixedArray : public IArray< T >
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
        FixedArray();
        FixedArray( SizeType size, ConstReference value = ValueType() );
        FixedArray( ConstIterator v, SizeType n );
        FixedArray( ConstIterator begin, ConstIterator end );
        ~FixedArray();

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

        void            Resize( SizeType size, ConstReference value = ValueType() );

        void            PushBackEmpty();
        void            PushBack( ConstReference value );
        void            PushBack( ConstPointer p, SizeType n );
        void            PushBack( ConstIterator begin, ConstIterator end );

        void            PopBack();
        void            Clear();

    protected:
        ValueType 	    m_begin[ C ];
        ValueType *	    m_end;

    private:
        // forbidden
        FixedArray( const FixedArray& other );
        FixedArray&     operator=( const FixedArray& other );

    };

    //======================================================================== FixedArray

    template< typename T, SizeT C >
    FixedArray< T, C >::FixedArray()
    {
        m_end = m_begin;
    }

    template< typename T, SizeT C >
    FixedArray< T, C >::FixedArray( SizeType size, ConstReference value = ValueType() )
    {
        CARBON_ASSERT( size <= C );

        m_end = m_begin + size;

        if ( size > 0 )
        {
            Fill( m_begin, m_end, value );
        }
    }

    template< typename T, SizeT C >
    FixedArray< T, C >::FixedArray( ConstIterator v, SizeType n )
    {
        CARBON_ASSERT( n <= C );

        m_end = m_begin + n;

        if ( n > 0 )
        {
            Copy( v, v+n, m_begin );
        }
    }

    template< typename T, SizeT C >
    FixedArray< T, C >::FixedArray( ConstIterator begin, ConstIterator end )
    {
        const SizeType size = static_cast< SizeType >( end - begin );

        CARBON_ASSERT( size <= C );

        m_end = m_begin + n;

        if ( size > 0 )
        {
            Copy( begin, end, m_begin );
        }
    }

    template< typename T, SizeT C >
    FixedArray< T, C >::~FixedArray()
    {
        Clear();
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::ConstPointer FixedArray< T, C >::ConstPtr() const
    {
        return m_begin;
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::Pointer FixedArray< T, C >::Ptr()
    {
        return m_begin;
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::ConstIterator FixedArray< T, C >::Begin() const
    {
        return m_begin;
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::Iterator FixedArray< T, C >::Begin()
    {
        return m_begin;
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::ConstIterator FixedArray< T, C >::End() const
    {
        return m_end;
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::Iterator FixedArray< T, C >::End()
    {
        return m_end;
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::ConstReference FixedArray< T, C >::Front() const
    {
        CARBON_ASSERT( m_begin != m_end );
        return *m_begin;
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::Reference FixedArray< T, C >::Front()
    {
        CARBON_ASSERT( m_begin != m_end );
        return *m_begin;
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::ConstReference FixedArray< T, C >::Back() const
    {
        CARBON_ASSERT( m_begin != m_end );
        return *( m_end - 1 );
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::Reference FixedArray< T, C >::Back()
    {
        CARBON_ASSERT( m_begin != m_end );
        return *( m_end - 1 );
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::ConstReference FixedArray< T, C >::At( SizeType index ) const
    {
        CARBON_ASSERT( index < Size() );
        return *( m_begin + index );
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::Reference FixedArray< T, C >::At( SizeType index )
    {
        CARBON_ASSERT( index < Size() );
        return *( m_begin + index );
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::ConstReference FixedArray< T, C >::operator[]( SizeType index ) const
    {
        return At( index );
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::Reference FixedArray< T, C >::operator[]( SizeType index )
    {
        return At( index );
    }

    template< typename T, SizeT C >
    Bool FixedArray< T, C >::Empty() const
    {
        return ( m_begin == m_end );
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::SizeType FixedArray< T, C >::Size() const
    {
        return static_cast< SizeType >( m_end - m_begin );
    }

    template< typename T, SizeT C >
    typename FixedArray< T, C >::SizeType FixedArray< T, C >::Capacity() const
    {
        return static_cast< SizeType >( C );
    }

    template< typename T, SizeT C >
    void FixedArray< T, C >::Resize( SizeType size, ConstReference value )
    {
        CARBON_ASSERT( size <= C );

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
            Fill( m_end, end, value );
            m_end = end;
        }
    }

    template< typename T, SizeT C >
    void FixedArray< T, C >::PushBackEmpty()
    {
        CARBON_ASSERT( m_end < ( m_begin + C ) );

        *m_end = ValueType();
        ++m_end;
    }

    template< typename T, SizeT C >
    void FixedArray< T, C >::PushBack( ConstReference value )
    {
        CARBON_ASSERT( m_end < ( m_begin + C ) );

        *m_end = value;
        ++m_end;
    }

    template< typename T, SizeT C >
    void FixedArray< T, C >::PushBack( ConstPointer p, SizeType n )
    {
        PushBack( p, p + n );
    }

    template< typename T, SizeT C >
    void FixedArray< T, C >::PushBack( ConstIterator begin, ConstIterator end )
    {
        const SizeType capacity = Size() + ( end - begin );

        CARBON_ASSERT( capacity <= C );

        Copy( begin, end, m_end );
        m_end = m_begin + capacity;
    }

    template< typename T, SizeT C >
    void FixedArray< T, C >::PopBack()
    {
        CARBON_ASSERT( m_begin < m_end );
        --m_end;
        m_end->~ValueType();
    }

    template< typename T, SizeT C >
    void FixedArray< T, C >::Clear()
    {
        while ( m_begin != m_end )
        {
            PopBack();
        }
    }

    //======================================================================== FixedArray
}

#endif // _CORE_FIXEDARRAY_H