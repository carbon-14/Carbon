#pragma once
#ifndef _CORE_ASSERT_H
#define _CORE_ASSERT_H

//
// Compile Time Assert from: http://www.jaggersoft.com/pubs/CVu11_3.html
//
// Assert from: http://cnicholson.net/2009/02/stupid-c-tricks-adventures-in-assert/
//
// Thx guys

#include "DLL.h"

_CoreExport void __CarbonAssertFail( const char * predicate, const char * file, int line, const char * msg );

#ifdef CARBON_DEBUG

    #if defined( CARBON_PLATFORM_WIN32 )
        #define CARBON_HALT() __debugbreak()
    #endif

    #define CARBON_COMPILE_TIME_ASSERT( predicate ) switch(0){ case 0:; case ( predicate ):; }

    #define CARBON_ASSERT( predicate ) \
        do { \
            if ( !( predicate ) ) { \
                __CarbonAssertFail( #predicate, __FILE__, __LINE__, 0 ); \
                CARBON_HALT(); \
            } \
        } while(0)

    #define CARBON_ASSERT_MESSAGE( predicate, msg ) \
        do { \
            if ( !( predicate ) ) { \
                __CarbonAssertFail( #predicate, __FILE__, __LINE__, (msg) ); \
                CARBON_HALT(); \
            } \
        } while(0)

#else

    #define CARBON_VAR_UNUSED(x) do { (void)sizeof(x); } while(0)

    #define CARBON_COMPILE_TIME_ASSERT( predicate ) do { CARBON_VAR_UNUSED( predicate ); } while(0)

    #define CARBON_ASSERT( predicate ) do { CARBON_VAR_UNUSED( predicate ); } while(0)

    #define CARBON_ASSERT_MESSAGE( predicate, msg ) do { CARBON_VAR_UNUSED( predicate ); CARBON_VAR_UNUSED( msg ); } while(0)

#endif

#endif // _CORE_ASSERT_H
