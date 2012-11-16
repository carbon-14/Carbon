#pragma once
#ifndef _CORE_FILESYSTEM_H
#define _CORE_FILESYSTEM_H

#include "Core/DLL.h"
#include "Core/Types.h"

#include "Core/FixedString.h"
#include "Core/Array.h"

namespace Core
{
    class _CoreExport FileSystem
    {
    public:
        enum PathType
        {
            PT_ROOT = 0,
            PT_DATA,
            PT_CACHE
        };

    public:
        static void Initialize( const Char * rootPath, const Char * dataDir = "data", const Char * cacheDir = "cache" );
        static void Destroy();

        static void BuildPathName( const Char * src, PathString& dest, PathType relativeTo = PT_ROOT );

        static Bool Load( const PathString& fileName, void *& buffer, SizeT& size );
        static Bool Save( const PathString& fileName, const void * buffer, SizeT size );

        static Bool Exists( const PathString& fileName );
        static Bool Find( const Char * searchStr, Core::Array< PathString >& fileNames, Bool absolute = true );
        static U64  GetLastWriteTime( const PathString& fileName );
    };
}

#endif // _CORE_FILESYSTEM_H
