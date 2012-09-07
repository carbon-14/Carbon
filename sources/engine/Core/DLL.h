#pragma once
#ifndef _CORE_DLL_H
#define _CORE_DLL_H

#if defined( CARBON_PLATFORM_WIN32 )
	#if defined( CARBON_CORE_DLLEXPORT )
		#define _CoreExport __declspec( dllexport )
	#else
		#define _CoreExport __declspec( dllimport )
	#endif
#else
	#define _CoreExport
#endif

#endif // _CORE_DLL_H