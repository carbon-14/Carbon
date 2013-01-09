#pragma once
#ifndef _GRAPHIC_PROGRAM_H
#define _GRAPHIC_PROGRAM_H

#include "RenderDevice.h"

namespace Graphic
{
    struct ProgramSet
    {
        U32     m_id;
        Handle  m_uniformBuffer;
    };

    class _GraphicExport Program
    {
    public:
        Program();
        Program( U32 id, U32 type, const Char * name );
        ~Program();

        Bool operator<( const Program& other )   { return m_id < other.m_id; }
        Bool operator==( const Program& other )  { return m_id == other.m_id;}

        U32     m_id;
        U32     m_type;
        Char    m_name[32];

        Handle  m_handle;

        SizeT   m_samplers[ RenderDevice::ms_maxTextureUnitCount ];
        SizeT   m_samplerCount;
    };
}

#endif // _GRAPHIC_PROGRAM_H
