#pragma once
#ifndef _CORE_DLL_H
#define _CORE_DLL_H

#if defined( CARBON_PLATFORM_WIN32 )
    #if defined( CARBON_RETAIL )
        #define _CoreExport
    #elif defined( CARBON_CORE_DLLEXPORT )
        #define _CoreExport __declspec( dllexport )
    #else
        #define _CoreExport __declspec( dllimport )
    #endif

    #pragma warning( disable: 4251 )    // disable warning of non-exported templates instances used as members in exported classes
#else
    #define _CoreExport
#endif

#endif // _CORE_DLL_H
