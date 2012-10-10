#pragma once
#ifndef _CORE_INTRINSICS_H
#define _CORE_INTRINSICS_H

#if defined( CARBON_PLATFORM_WIN32 )

#include <xmmintrin.h>

typedef __m128	M128;

#else

#error Intrinsics not defined

#endif


#endif // _CORE_INTRINSICS_H
