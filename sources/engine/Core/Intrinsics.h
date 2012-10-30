#pragma once
#ifndef _CORE_INTRINSICS_H
#define _CORE_INTRINSICS_H

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Core/ps/win32/Intrinsics.inl"
#else
    #error Intrinsics not defined
#endif

#endif // _CORE_INTRINSICS_H
