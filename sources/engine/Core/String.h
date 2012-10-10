#pragma once
#ifndef _CORE_STRING_H
#define _CORE_STRING_H

#include "Core/Array.h"

namespace Core
{
    //====================================================================================
    // StringImpl
    //====================================================================================

    template< typename Alloc = DefaultAllocator >
    class StringImpl : public Array< Char, Alloc >
    {
    public:
        typedef Char                    ValueType;
        typedef Char *                  Pointer;
        typedef const Char *            ConstPointer;
        typedef Char&                   Reference;
        typedef const Char&             ConstReference;
        typedef SizeT                   SizeType;
        typedef Char *                  Iterator;
        typedef const Char *            ConstIterator;
        typedef Array< Char, Alloc >    Super;

        static const SizeT              ms_MinStringSize = 8;

    public:
        StringImpl();
        StringImpl( ConstPointer str );

        StringImpl& Append( ConstPointer str );
        StringImpl& Append( ConstPointer str, SizeType len );
        StringImpl& operator+=( ConstPointer str );

    private:
        StringImpl( const StringImpl& other );
        StringImpl& operator=( const StringImpl& other );
    };

    //============================================================================== StringImpl

    template< typename Alloc >
    StringImpl< Alloc >::StringImpl()
    {
        m_begin = m_end = DoAllocate( ms_MinStringSize );
        m_capacity = m_begin + ms_MinStringSize;
        MemoryUtils::MemSet( m_begin, 0, ms_MinStringSize );
    }

    template< typename Alloc >
    StringImpl< Alloc >::StringImpl( ConstPointer str )
    {
        const SizeType size = StringUtils::StrLen( str );
        if ( size > 0 )
        {
            const SizeType capacity = size + 1;
            m_begin = DoAllocate( capacity );
            m_end = m_begin + size;
            m_capacity = m_begin + capacity;
            MemoryUtils::MemCpy( m_begin, str, size );
            *m_end = 0;
        }
        else
        {
            m_begin = m_end = DoAllocate( ms_MinStringSize );
            m_capacity = m_begin + ms_MinStringSize;
            MemoryUtils::MemSet( m_begin, 0, ms_MinStringSize );
        }
    }

    template< typename Alloc >
    StringImpl< Alloc >& StringImpl< Alloc >::Append( ConstPointer str )
    {
        return Append( str, StringUtils::StrLen( str ) );
    }
template< typename Alloc >
    StringImpl< Alloc >& StringImpl< Alloc >::Append( ConstPointer str, SizeType len )
    {
        if ( len > 0 )
        {
            const SizeType size = Size() + len;
            const SizeType capacity = Size() + len + 1;
            if ( ( m_begin + capacity ) > m_capacity )
            {
                IncreaseCapacity( capacity );
            }
            MemoryUtils::MemCpy( m_begin, str, len );
            m_end = m_begin + size;
            *m_end = 0;
        }

        return *this;
    }


    template< typename Alloc >
    StringImpl< Alloc >& StringImpl< Alloc >::operator+=( ConstPointer str )
    {
        return Append( str );
    }

    //============================================================================== StringImpl

    //====================================================================================
    // String
    //====================================================================================

    typedef StringImpl< > String;
}

#endif // _CORE_STRING_H
