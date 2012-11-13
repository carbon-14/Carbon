#pragma once
#ifndef _GRAPHIC_PROGRAMCACHE_H
#define _GRAPHIC_PROGRAMCACHE_H

#include "Graphic/Program.h"

#include "Core/FixedString.h"
#include "Core/Array.h"

namespace Graphic
{
    typedef SizeT ProgramHandle;

    class _GraphicExport ProgramCache
    {
    public:
        friend class RenderDevice;

    public:
        static const ProgramHandle ms_invalidHandle;

        Bool            Initialize( const Char * relativePath );
        void            Destroy();
        void            Update();

        ProgramHandle   GetProgram( const Char * name ) const;

        void            UseProgram( ProgramHandle handle ) const;

        static void     NotifySourceChange();

    private:
        void            BuildCache();
        void            ReloadCache();

        void            LoadProgram( Program& program );
        void            LoadProgramFromSources( Program& program );
        void            LoadProgramFromBinaries( Program& program );

        static U32      CreateId( const Char * name );
        ProgramHandle   FindById( U32 id ) const;

        typedef Core::Array< Graphic::Program > ProgramArray;

        Core::PathString    m_dataPath;
        Core::PathString    m_cachePath;
        ProgramArray        m_cache;
    };
}

#endif // _GRAPHIC_PROGRAMCACHE_H
