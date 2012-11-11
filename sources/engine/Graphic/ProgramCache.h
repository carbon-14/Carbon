#pragma once
#ifndef _GRAPHIC_PROGRAMCACHE_H
#define _GRAPHIC_PROGRAMCACHE_H

#include "Graphic/DLL.h"

#include "Graphic/Program.h"

#include "Core/FixedString.h"
#include "Core/Array.h"

namespace Graphic
{
    class _GraphicExport ProgramCache
    {
    public:
        static const SizeT ms_InvalidIndex = -1;

        Bool            Initialize( const Char * relativePath );
        void            Destroy();
        void            Update();

        SizeT           GetIndex( const Char * name );

        U32             GetHandle( SizeT index ) const;

        static void     NotifySourceChange();

    private:
        void            BuildCache();
        void            ReloadCache();

        void            LoadProgram( Program& program );
        void            LoadProgramFromSources( Program& program );
        void            LoadProgramFromBinaries( Program& program );

        U32             CreateId( const Char * name );
        Program *       FindById( U32 id );

        typedef Core::Array< Graphic::Program > ProgramArray;

        Core::PathString    m_dataPath;
        Core::PathString    m_cachePath;
        ProgramArray        m_cache;
    };
}

#endif // _GRAPHIC_PROGRAMCACHE_H
