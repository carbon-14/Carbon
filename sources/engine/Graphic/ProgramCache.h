#pragma once
#ifndef _GRAPHIC_PROGRAMCACHE_H
#define _GRAPHIC_PROGRAMCACHE_H

#include "Graphic/Program.h"

#include "Core/FixedString.h"
#include "Core/Array.h"
#include "Core/String.h"

namespace Graphic
{
    class MaterialResource;

    typedef U16 ProgramHandle;

    class _GraphicExport ProgramCache
    {
    public:
        friend class RenderDevice;

    public:
        static const ProgramHandle      ms_invalidHandle;
        static const SizeT              ms_tileSize;

        static Bool                     Initialize( const Char * shaderPath );
        static void                     Destroy();
        static void                     Update();

        static U32                      CreateId( const Char * str );

        static ProgramHandle            GetProgram( U32 nameId, U32 setId = CreateId( "" ) );
        static MaterialResource *       CreateMaterial( U32 materialId );

        static void                     UseProgram( ProgramHandle handle );

        static void                     NotifySourceChange();

    private:
        typedef Array< Program >        ProgramArray;
        typedef Array< ProgramSet >     ProgramSetArray;
        typedef Array< Handle >         SamplerArray;

        static void                     BuildCache();
        static void                     ReloadCache();

        static void                     LoadSamplerList();
        static void                     LoadProgramHeaders( String& headers );
        static void                     LoadProgram( Program& program, const Char * headers );
        static void                     LoadProgramFromSources( Program& program, const Char * headers );
        static void                     LoadProgramFromBinaries( Program& program );
        static void                     LoadProgramSets();

        static ProgramArray::Iterator   Find( U32 id );

        static PathString               m_dataPath;
        static PathString               m_cachePath;
        static PathString               m_materialPath;

        static ProgramArray             m_programs;
        static ProgramSetArray          m_programSets;
        static SamplerArray             m_samplers;
    };
}

#endif // _GRAPHIC_PROGRAMCACHE_H
