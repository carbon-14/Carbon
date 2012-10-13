#include "Core/Math.h"

#include <cmath>

namespace Core
{
    const F32 pi_f32        = 4.0f * atanf( 1.0f );
    const F32 twoPi_f32     = 8.0f * atanf( 1.0f );
    const F32 halfPi_f32    = 2.0f * atanf( 1.0f );

    inline F32 Math::Pi()                                   { return pi_f32;                            }
    inline F32 Math::TwoPi()                                { return twoPi_f32;                         }
    inline F32 Math::HalfPi()                               { return halfPi_f32;                        }

    inline S32 Math::IAbs( S32 value )                      { return abs( value );                      }
    inline S32 Math::IMin( S32 a, S32 b )                   { return ( a < b ) ? a : b;                 }
    inline S32 Math::IMax( S32 a, S32 b )                   { return ( a < b ) ? b : a;                 }
    inline S32 Math::IClamp( S32 value, S32 min, S32 max )  { return IMax( min, IMin( max, value ) );   }

    inline F32 Math::Abs( F32 value )                       { return fabsf( value );                    }
    inline F32 Math::Min( F32 a, F32 b )                    { return ( a < b ) ? a : b;                 }
    inline F32 Math::Max( F32 a, F32 b )                    { return ( a < b ) ? b : a;                 }
    inline F32 Math::Clamp( F32 value, F32 min, F32 max )   { return Max( min, Min( max, value ) );     }
    inline F32 Math::Floor( F32 value )                     { return floorf( value );                   }
    inline F32 Math::Ceil( F32 value )                      { return ceilf( value );                    }
    inline F32 Math::Lerp( F32 left, F32 right, F32 ratio ) { return left + ( right - left ) * ratio;   }

    inline F32 Pow( F32 value, F32 exponent )               { return powf( value, exponent );           }
    inline F32 Math::Sqrt( F32 value )                      { return sqrtf( value );                    }
    inline F32 Math::Exp( F32 value )                       { return expf( value );                     }
    inline F32 Math::Log( F32 value )                       { return logf( value );                     }

    inline F32 Math::Sin( F32 angle )                       { return sinf( angle );                     }
    inline F32 Math::Cos( F32 angle )                       { return cosf( angle );                     }
    inline F32 Math::Tan( F32 angle )                       { return tanf( angle );                     }
    inline F32 Math::ASin( F32 value )                      { return asinf( value );                    }
    inline F32 Math::ACos( F32 value )                      { return acosf( value );                    }
    inline F32 Math::ATan( F32 value )                      { return atanf( value );                    }
}
