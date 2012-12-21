#pragma once
#ifndef _CORE_TYPETRAITS_H
#define _CORE_TYPETRAITS_H

#include "Core/Types.h"

namespace Core
{
    template< typename T, T v >
    struct IntegralConstant
    {
        static const T                      value = v;
        typedef T                           ValueType;
        typedef IntegralConstant< T, v >    Type;
    };

    typedef IntegralConstant< Bool, true >  TrueType;
    typedef IntegralConstant< Bool, false > FalseType;

    template< typename T >
    struct IsIntegral : public FalseType { };

    template< > struct IsIntegral< S8 >     : public TrueType { };
    template< > struct IsIntegral< U8 >     : public TrueType { };
    template< > struct IsIntegral< S16 >    : public TrueType { };
    template< > struct IsIntegral< U16 >    : public TrueType { };
    template< > struct IsIntegral< S32 >    : public TrueType { };
    template< > struct IsIntegral< U32 >    : public TrueType { };
    template< > struct IsIntegral< S64 >    : public TrueType { };
    template< > struct IsIntegral< U64 >    : public TrueType { };
    template< > struct IsIntegral< Bool >   : public TrueType { };
    template< > struct IsIntegral< Char >   : public TrueType { };

    template< typename T >
    struct IsFloating : public FalseType { };

    template< > struct IsFloating< F32 >    : public TrueType { };
    template< > struct IsFloating< F64 >    : public TrueType { };

    template< typename T >
    struct IsPOD : public IntegralConstant< Bool, IsIntegral< T >::value || IsFloating< T >::value > { };

    template< typename T >
    struct IsPOD< T* >  : public TrueType { };

    template< typename T >
    struct IsPOD< T* const >  : public TrueType { };
}

#define CARBON_DECLARE_POD_TYPE( T ) \
    template< > struct Core::IsPOD< T > : public TrueType { }

#endif // _CORE_TYPETRAITS_H
