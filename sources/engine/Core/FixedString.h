#pragma once
#ifndef _CORE_FIXEDSTRING_H
#define _CORE_FIXEDSTRING_H

#include "Core/DLL.h"

#include "Core/FixedArray.h"
#include "Core/MemoryUtils.h"
#include "Core/StringUtils.h"

namespace Core
{
    //====================================================================================
    // FixedString
    //====================================================================================

    template< SizeT N >
    class FixedString : public FixedArray< Char, N >
    {
    public:
        typedef Char            ValueType;
        typedef Char *          Pointer;
        typedef const Char *    ConstPointer;
        typedef Char&           Reference;
        typedef const Char&     ConstReference;
        typedef SizeT           SizeType;
        typedef Char *          Iterator;
        typedef const Char *    ConstIterator;

    public:
        FixedString();
        FixedString( ConstPointer str );
        FixedString( ConstPointer str, SizeType len );
        FixedString( const FixedString& other );

        FixedString& operator=( const FixedString& other );

        FixedString& Append( ConstPointer str );
        FixedString& Append( ConstPointer str, SizeType len );
        FixedString& operator+=( ConstPointer str );

        void Copy( Char * dest, SizeT size );        
    };

    //============================================================================== FixedString

    template< SizeT N >
    FixedString< N >::FixedString()
    {
        m_end = m_begin;
        *m_end = 0;
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
        }
        else
        {
            m_end = m_begin;
        }

        *m_end = 0;
    }

    template< SizeT N >
    FixedString< N >::FixedString( ConstPointer str, SizeType len )
    {
        CARBON_ASSERT( len < N );

        if ( len > 0 )
        {
            m_end = m_begin + len;
            MemoryUtils::MemCpy( m_begin, str, len );
        }
        else
        {
            m_end = m_begin;
        }

        *m_end = 0;
    }

    template< SizeT N >
    FixedString< N >::FixedString( const FixedString& other )
    {
        const SizeT size = other.Size();

        if ( size > 0 )
        {
            m_end = m_begin + size;
            MemoryUtils::MemCpy( m_begin, other.Begin(), size );
        }
        else
        {
            m_end = m_begin;
        }

        *m_end = 0;
    }

    template< SizeT N >
    FixedString< N >& FixedString< N >::operator=( const FixedString& other )
    {
        const SizeT size = other.Size();

        if ( size > 0 )
        {
            m_end = m_begin + size;
            MemoryUtils::MemCpy( m_begin, other.Begin(), size );
        }
        else
        {
            m_end = m_begin;
        }

        *m_end = 0;

        return *this;
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

            MemoryUtils::MemCpy( m_end, str, len );
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

    template< typename SizeT N >
    void FixedString< N >::Copy( Char * dest, SizeT size )
    {
        CARBON_ASSERT( Size() < size );

        MemoryUtils::MemCpy( dest, m_begin, Size() );
        dest[ Size() ] = 0;
    }

    //============================================================================== FixedString

    typedef FixedString< 32 >   FileString;
    typedef FixedString< 256 >  PathString;
}

#endif // _CORE_FIXEDSTRING_H
