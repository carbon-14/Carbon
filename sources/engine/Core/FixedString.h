#pragma once
#ifndef _CORE_FIXEDSTRING_H
#define _CORE_FIXEDSTRING_H

#include "Core/FixedArray.h"

namespace Core
{
    //====================================================================================
    // FixedString
    //====================================================================================

    template< SizeT N >
    class FixedString : public FixedArray< Char, N >
    {
    public:
        typedef FixedArray< Char, N >    Super;

    public:
        FixedString();
        FixedString( ConstPointer str );

        FixedString& Append( ConstPointer str );
        FixedString& Append( ConstPointer str, SizeType len );
        FixedString& operator+=( ConstPointer str );

    private:
        FixedString( const FixedString& other );
        FixedString& operator=( const FixedString& other );
    };

    //============================================================================== FixedString

    template< SizeT N >
    FixedString< N >::FixedString()
    {
        m_end = m_begin;
        MemoryUtils::MemSet( m_begin, 0, N );
    }

    template< SizeT N >
    FixedString< N >::FixedString( ConstPointer str )
    {
        const SizeType size = StringUtils::StrLen( str );

        CARBON_ASSERT( size < N );

        if ( size > 0 )
        {
            m_end = m_begin + size;
            MemoryUtils::MemCpy( m_begin, str, size );
            *m_end = 0;
        }
        else
        {
            m_end = m_begin;
            MemoryUtils::MemSet( m_begin, 0, N );
        }
    }

    template< SizeT N >
    FixedString< N >& FixedString< N >::Append( ConstPointer str )
    {
        return Append( str, StringUtils::StrLen( str ) );
    }

    template< SizeT N >
    FixedString< N >& FixedString< N >::Append( ConstPointer str, SizeType len )
    {
        if ( len > 0 )
        {
            const SizeType size = Size() + len;

            CARBON_ASSERT( size < N );

            MemoryUtils::MemCpy( m_begin, str, len );
            m_end = m_begin + size;
            *m_end = 0;
        }

        return *this;
    }

    template< SizeT N >
    FixedString< N >& FixedString< N >::operator+=( ConstPointer str )
    {
        return Append( str );
    }

    //============================================================================== FixedString
}

#endif // _CORE_FIXEDSTRING_H
