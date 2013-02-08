#pragma once
#ifndef _CORE_FIXEDSTRING_H
#define _CORE_FIXEDSTRING_H

#include "Core/DLL.h"

#include "Core/FixedArray.h"
#include "Core/MemoryUtils.h"
#include "Core/StringUtils.h"

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
    : FixedArray< Char, N >( str, StringUtils::StrLen( str ) )
{
    CARBON_ASSERT( Size() < N );
    *m_end = 0;
}

template< SizeT N >
FixedString< N >::FixedString( ConstPointer str, SizeType len )
    : FixedArray< Char, N >( str, len )
{
    CARBON_ASSERT( Size() < N );
    *m_end = 0;
}

template< SizeT N >
FixedString< N >::FixedString( const FixedString& other )
    : FixedArray< Char, N >( other )
{
    CARBON_ASSERT( Size() < N );
    *m_end = 0;
}

template< SizeT N >
FixedString< N >& FixedString< N >::operator=( const FixedString& other )
{
    Clear();
    Append( other.m_begin, other.Size() );

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
        PushBack( str, len );

        CARBON_ASSERT( Size() < N );
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

typedef FixedString< 32 >   FileString;
typedef FixedString< 256 >  PathString;

#endif // _CORE_FIXEDSTRING_H
