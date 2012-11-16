#pragma once
#ifndef _CORE_HASH_H
#define _CORE_HASH_H

namespace Core
{
    U32 HashString( const Char * str )
    {
        // FNV hash
        // http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
        // Thx !
        //
        U32 h = 2166136261;

        Char c;
        while ( (c = *(++str)) != 0 ) // be sure that the string ends by '\0'
        {
            h = ( h * 16777619 ) ^ c;
        }
        return h;
    };
}

#endif // _CORE_HASH_H
