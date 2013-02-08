#include <cmath>

#include "Core/Types.h"

inline F32 Pi()                                   { return 4.0f * atanf( 1.0f );              }
inline F32 TwoPi()                                { return 8.0f * atanf( 1.0f );              }
inline F32 HalfPi()                               { return 2.0f * atanf( 1.0f );              }

inline S32 IAbs( S32 value )                      { return abs( value );                      }
inline S32 IMin( S32 a, S32 b )                   { return ( a < b ) ? a : b;                 }
inline S32 IMax( S32 a, S32 b )                   { return ( a < b ) ? b : a;                 }
inline S32 IClamp( S32 value, S32 min, S32 max )  { return IMax( min, IMin( max, value ) );   }

inline F32 Abs( F32 value )                       { return fabsf( value );                    }
inline F32 Min( F32 a, F32 b )                    { return ( a < b ) ? a : b;                 }
inline F32 Max( F32 a, F32 b )                    { return ( a < b ) ? b : a;                 }
inline F32 Clamp( F32 value, F32 min, F32 max )   { return Max( min, Min( max, value ) );     }
inline F32 Floor( F32 value )                     { return floorf( value );                   }
inline F32 Ceil( F32 value )                      { return ceilf( value );                    }
inline F32 Lerp( F32 left, F32 right, F32 ratio ) { return left + ( right - left ) * ratio;   }

inline F32 Sqrt( F32 value )                      { return sqrtf( value );                    }
inline F32 Exp( F32 value )                       { return expf( value );                     }
inline F32 Log( F32 value )                       { return logf( value );                     }

inline F32 Sin( F32 angle )                       { return sinf( angle );                     }
inline F32 Cos( F32 angle )                       { return cosf( angle );                     }
inline F32 Tan( F32 angle )                       { return tanf( angle );                     }
inline F32 ASin( F32 value )                      { return asinf( value );                    }
inline F32 ACos( F32 value )                      { return acosf( value );                    }
inline F32 ATan( F32 value )                      { return atanf( value );                    }
