#pragma once
#ifndef _CORE_MATH_H
#define _CORE_MATH_H

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Core/ps/win32/Math.inl"
#else
    #error Math not defined
#endif

#endif // _CORE_MATH_H
