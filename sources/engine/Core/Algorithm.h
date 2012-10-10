#pragma once
#ifndef _CORE_ALGORITHM_H
#define _CORE_ALGORITHM_H

namespace Core
{
    template < typename Iterator, typename ConstIterator, typename T >
    void Fill( Iterator begin, ConstIterator end, const T& value )
    {
        for ( ; begin != end; ++begin )
        {
            *begin = value;
        }
    }

    template< typename ConstIterator, typename Iterator >
    void Copy( ConstIterator begin, ConstIterator end, Iterator to )
    {
        for ( ; begin != end; ++begin, ++to )
        {
            *to = *begin;
        }
    }
}

#endif // _CORE_ALGORITHM_H
