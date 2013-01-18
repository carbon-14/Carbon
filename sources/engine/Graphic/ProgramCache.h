#pragma once
#ifndef _GRAPHIC_PROGRAMCACHE_H
#define _GRAPHIC_PROGRAMCACHE_H

#include "Graphic/Program.h"

#include "Core/FixedString.h"
#include "Core/Array.h"

namespace Graphic
{
    class MaterialResource;

    typedef U16 ProgramHandle;

    class _GraphicExport ProgramCache
    {
    public:
        friend class RenderDevice;

    public:
        static const ProgramHandle          ms_invalidHandle;

        static Bool                         Initialize( const Char * shaderPath );
        static void                         Destroy();
        static void                         Update();

        static U32                          CreateId( const Char * str );

        static ProgramHandle                GetProgram( U32 nameId, U32 setId = CreateId( "" ) );
        static MaterialResource *           CreateMaterial( U32 materialId );

        static void                         UseProgram( ProgramHandle handle );

        static void                         NotifySourceChange();

    private:
        typedef Core::Array< Program >      ProgramArray;
        typedef Core::Array< ProgramSet >   ProgramSetArray;
        typedef Core::Array< Handle >       SamplerArray;

        static void                         BuildCache();
        static void                         ReloadCache();

        static void                         LoadSamplerList();
        static void                         LoadProgram( Program& program );
        static void                         LoadProgramFromSources( Program& program );
        static void                         LoadProgramFromBinaries( Program& program );
        static void                         LoadProgramSets();

        static ProgramArray::Iterator       Find( U32 id );

        static void                         SetProgram( Handle program );
        static void                         SetSampler( Handle sampler, SizeT index );
        static void                         SetUniformBuffer( Handle uniformBuffer );

        static Core::PathString             m_dataPath;
        static Core::PathString             m_cachePath;
        static Core::PathString             m_materialPath;

        static ProgramArray                 m_programs;
        static ProgramSetArray              m_programSets;
        static SamplerArray                 m_samplers;

        static Handle                       m_programCache;
        static Handle                       m_samplerCache[ RenderDevice::ms_maxTextureUnitCount ];
        static Handle                       m_uniformBufferCache;
    };
}

#endif // _GRAPHIC_PROGRAMCACHE_H
