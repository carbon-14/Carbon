#pragma once
#ifndef _GRAPHIC_PROGRAMCACHE_H
#define _GRAPHIC_PROGRAMCACHE_H

#include "Graphic/Program.h"

#include "Core/FixedString.h"
#include "Core/Array.h"

namespace Graphic
{
    typedef U16 ProgramHandle;

    class _GraphicExport ProgramCache
    {
    public:
        friend class RenderDevice;

    public:
        static const ProgramHandle  ms_invalidHandle;

        Bool                        Initialize( const Char * shaderPath, const Char * materialPath );
        void                        Destroy();
        void                        Update();

        ProgramHandle               GetProgram( const Char * name, const Char * set = "" ) const;
        ProgramHandle               GetProgram( U32 nameId, U32 setId ) const;

        void                        UseProgram( ProgramHandle handle );

        static void                 NotifySourceChange();

    private:
        typedef Core::Array< Program > ProgramArray;
        typedef Core::Array< ProgramSet > ProgramSetArray;
        typedef Core::Array< Handle > SamplerArray;

        void                        BuildCache();
        void                        ReloadCache();

        void                        LoadSamplerList();
        void                        LoadProgram( Program& program );
        void                        LoadProgramFromSources( Program& program );
        void                        LoadProgramFromBinaries( Program& program );
        void                        LoadProgramSets();

        static U32                  CreateId( const Char * str );
        ProgramArray::ConstIterator Find( U32 id ) const;
        ProgramArray::Iterator      Find( U32 id );

        void                        SetProgram( Handle program );
        void                        SetSamplers( const SizeT * samplers, SizeT count );
        void                        SetUniformBuffer( Handle uniformBuffer );

        Core::PathString            m_dataPath;
        Core::PathString            m_cachePath;
        Core::PathString            m_materialPath;

        ProgramArray                m_programs;
        ProgramSetArray             m_programSets;
        SamplerArray                m_samplers;

        Handle                      m_programCache;
        SizeT                       m_samplerCache[ RenderDevice::ms_maxTextureUnitCount ];
        SizeT                       m_samplerCacheCount;
        Handle                      m_uniformBufferCache;
    };
}

#endif // _GRAPHIC_PROGRAMCACHE_H
