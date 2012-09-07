#pragma once
#ifndef _CORE_PLATFORM_H
#define _CORE_PLATFORM_H

#if defined( CARBON_PLATFORM_WIN32 )
	#define CARBON_PLATFORM_NAME "win32"
#else
	#error  Could not determine your platform
#endif


#endif // _CORE_PLATFORM_H
