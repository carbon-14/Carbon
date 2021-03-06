#pragma once
#ifndef _CORE_TYPES_H
#define _CORE_TYPES_H

#if defined( CARBON_PLATFORM_WIN32 )

typedef signed char         S8;
typedef signed short        S16;
typedef signed long         S32;
typedef signed __int64      S64;
typedef unsigned char       U8;
typedef unsigned short      U16;
typedef unsigned long       U32;
typedef unsigned __int64    U64;
typedef float               F32;
typedef double              F64;

#else

#error Basic types not defined

#endif

typedef bool	Bool;
typedef char	Char;
typedef U32     SizeT;

#endif // _CORE_TYPES_H
