#pragma once
#ifndef _CORE_INTRINSICS_H
#define _CORE_INTRINSICS_H

#if defined( CARBON_PLATFORM_WIN32 )

#include <xmmintrin.h>

typedef __declspec(align(16)) float F128[4];    // storage usage
typedef __m128                      M128;       // compute usage

#else

#error Intrinsics not defined

#endif


#endif // _CORE_INTRINSICS_H
