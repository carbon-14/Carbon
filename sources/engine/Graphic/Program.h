#pragma once
#ifndef _GRAPHIC_PROGRAM_H
#define _GRAPHIC_PROGRAM_H

#include "Graphic/DLL.h"
#include "Graphic/Types.h"

#include "Core/TypeTraits.h"

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

        Bool operator<( const Program& other )   { return m_id < other.m_id;    }
        Bool operator==( const Program& other )  { return m_id == other.m_id;   }

        U32             m_id;
        U32             m_type;
        Char            m_name[32];

        Handle          m_handle;

        LayoutHandle    m_samplers[ s_maxTextureUnitCount ];
        SizeT           m_samplerCount;

        SizeT           m_setCount;
    };
}

CARBON_DECLARE_POD_TYPE( Graphic::ProgramSet );

#endif // _GRAPHIC_PROGRAM_H
