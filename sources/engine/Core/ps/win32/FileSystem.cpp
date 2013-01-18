#include "Core/FileSystem.h"

#include "Core/MemoryManager.h"
#include "Core/Trace.h"

#include <Windows.h>
#include <cstdio>

namespace Core
{
    static PathString rootPath;
    static PathString dataPath;
    static PathString cachePath;

    Bool RecursiveCreateDirectory( const Char * dir )
    {
        if ( ! CreateDirectory( dir, NULL ) )
        {
            DWORD err = GetLastError();
            if ( err == ERROR_ALREADY_EXISTS )
            {
                return true;
            }
            if ( err == ERROR_PATH_NOT_FOUND )
            {
                Char * fileStart;
                Char parentDir[ 256 ];
                if ( GetFullPathName( dir, 256, parentDir, &fileStart ) == 0 )
                {
                    return false;
                }
                *(fileStart - 1 ) = 0;

                if ( RecursiveCreateDirectory( parentDir ) )
                {
                    return ( CreateDirectory( dir, NULL ) != 0 );
                }

                return false;
            }

            return false;
        }

        return true;
    }

    void FileSystem::Initialize( const Char * root, const Char * dataDir, const Char * cacheDir )
    {
        CARBON_ASSERT( rootPath.Empty() );
        CARBON_ASSERT( dataPath.Empty() );
        CARBON_ASSERT( cachePath.Empty() );

        rootPath = root;

        dataPath = rootPath;
        dataPath += "/";
        dataPath += dataDir;
        dataPath += "/";

        cachePath = rootPath;
        cachePath += "/";
        cachePath += cacheDir;
        cachePath += "/";
    }

    void FileSystem::Destroy()
    {
        rootPath.Clear();
        dataPath.Clear();
        cachePath.Clear();
    }

    void FileSystem::BuildPathName( const Char * src, PathString& dest, PathType relativeTo )
    {
        switch ( relativeTo )
        {
        case PT_ROOT:
            dest = rootPath;
            break;
        case PT_DATA:
            dest = dataPath;
            break;
        case PT_CACHE:
            dest = cachePath;
            break;
        };

        dest += src;
    }

    Bool FileSystem::Load( const PathString& fileName, void *& buffer, SizeT& size )
    {
        FILE * pFile;
        if ( fopen_s( &pFile, fileName.ConstPtr(), "rb" ) )
        {
            return false;
        }

        fseek( pFile , 0 , SEEK_END );
        size = ftell( pFile );

        if ( size == 0 )
        {
            fclose( pFile );
            return true;
        }

        rewind( pFile );

        buffer = UnknownAllocator::Allocate( size );

        if ( fread( buffer, 1, size, pFile ) != size )
        {
            UnknownAllocator::Deallocate( buffer );
            size = 0;
            fclose( pFile );
            return false;
        }

        fclose(pFile);

        return true;
    }

    Bool FileSystem::Save( const PathString& fileName, const void * buffer, SizeT size )
    {
        Char dir[ 256 ];
        Char * fileStart;

        if ( GetFullPathName( fileName.ConstPtr(), 256, dir, &fileStart ) == 0 )
        {
            return false;
        }
        *( fileStart - 1 ) = 0;

        if ( ! RecursiveCreateDirectory( dir ) )
        {
            return false;
        }

        FILE * pFile;
        if ( fopen_s( &pFile, fileName.ConstPtr(), "wb" ) )
        {
            return false;
        }

        if ( size == 0 )
        {
            fclose( pFile );
            return true;
        }

        if ( fwrite( buffer, 1, size, pFile ) != size )
        {
            fclose( pFile );
            return false;
        }

        fclose(pFile);

        return true;
    }

    Bool FileSystem::Exists( const PathString& fileName )
    {
        HANDLE hFile = CreateFile( fileName.ConstPtr(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if ( hFile == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        CloseHandle( hFile );
        return true;
    }

    Bool FileSystem::Find( const Char * searchStr, Core::Array< PathString >& fileNames, Bool absolute )
    {
        WIN32_FIND_DATA ffd;
        HANDLE hFind;

        hFind = FindFirstFile( searchStr, &ffd );
        if ( hFind == INVALID_HANDLE_VALUE ) 
        {
            return false;
        }

        Char dir[ 256 ];
        Char * fileStart = dir;

        if ( absolute )
        {
            if ( GetFullPathName( searchStr, 256, dir, &fileStart ) == 0 )
            {
                return false;
            }
        }

        do
        {
            if ( ! ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
            {
                PathString f( dir, fileStart - dir );
                f += ffd.cFileName;
                fileNames.PushBack( f );
            }
        }
        while ( FindNextFile( hFind, &ffd ) != 0 );

        FindClose( hFind );

        return true;
    }

    U64 FileSystem::GetLastWriteTime( const PathString& fileName )
    {
        HANDLE hFile = CreateFile( fileName.ConstPtr(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        if ( hFile == INVALID_HANDLE_VALUE )
        {
            return 0;
        }

        FILETIME ftCreate, ftAccess, ftWrite;
        if ( !GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
        {
            CloseHandle(hFile);
            return 0;
        }

        CloseHandle(hFile);

        ULARGE_INTEGER tmp;
        tmp.LowPart     = ftWrite.dwLowDateTime;
        tmp.HighPart    = ftWrite.dwHighDateTime;
        
        return tmp.QuadPart;
    }
}
