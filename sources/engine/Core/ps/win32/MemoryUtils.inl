#include <new>

#include "Core/TypeTraits.h"

namespace Core
{
    template< typename T >
    SizeT MemoryUtils::AlignOf()
    {
        return __alignof( T );
    }

    template < typename T >
    void MemoryUtils::Fill( T * begin, const T * end, const T& value )
    {
        if ( IsPOD< T >::value )
        {
            for ( ; begin != end; ++begin )
            {
                *begin = value;
            }
        }
        else
        {
            for ( ; begin != end; ++begin )
            {
                new( begin ) T( value );
            }
        }
    }

    template< typename T >
    void MemoryUtils::Copy( const T * begin, const T * end, T * to )
    {
        if ( IsPOD< T >::value )
        {
            MemoryUtils::MemCpy( to, begin, sizeof(T) * ( end - begin ) );
        }
        else
        {
            for ( ; begin != end; ++begin, ++to )
            {
                new( to ) T( *begin );
            }
        }
    }
}
