#include "Core/Hash.h"

namespace Core
{
    U32 HashString( const Char * str )
    {
        // FNV hash
        // http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
        // Thx !
        //
        U32 h = 2166136261;

        while ( *(str++) != 0 ) // be sure that the string ends by '\0'
        {
            h = ( h * 16777619 ) ^ (*str);
        }
        return h;
    };
}
