#pragma once
#ifndef _GRAPHIC_PROGRAM_H
#define _GRAPHIC_PROGRAM_H

#include "Graphic/DLL.h"
#include "Core/Types.h"

namespace Graphic
{
    typedef SizeT ProgramHandle;

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
        U32     m_handle;
        Char    m_name[32];
    };
}

#endif // _GRAPHIC_PROGRAM_H
