#include <new>

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
        for ( ; begin != end; ++begin )
        {
            new( begin ) T( value );
        }
    }

    template< typename T >
    void MemoryUtils::Copy( const T * begin, const T * end, T * to )
    {
        for ( ; begin != end; ++begin, ++to )
        {
            new( to ) T( *begin );
        }
    }
}
