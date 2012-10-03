#pragma once
#ifndef _CORE_ALGORITHM_H
#define _CORE_ALGORITHM_H

template < typename Iterator, typename T >
void Fill( Iterator begin, Iterator end, const T& value )
{
    for ( ; begin != end; ++begin )
    {
        *begin = value;
    }
}

template< typename Iterator >
void Copy( Iterator begin, Iterator end, Iterator to )
{
    for ( ; begin != end; ++begin, ++to )
    {
        *to = *begin;
    }
}

#endif
