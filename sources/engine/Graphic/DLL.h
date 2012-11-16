#pragma once
#ifndef _GRAPHIC_DLL_H
#define _GRAPHIC_DLL_H

#if defined( CARBON_PLATFORM_WIN32 )
    #if defined( CARBON_RETAIL )
        #define _GraphicExport
    #elif defined( CARBON_GRAPHIC_DLLEXPORT )
        #define _GraphicExport __declspec( dllexport )
    #else
        #define _GraphicExport __declspec( dllimport )
    #endif
#else
    #define _GraphicExport
#endif



#endif // _GRAPHIC_DLL_H
