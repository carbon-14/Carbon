#include "Graphic/Program.h"

#include "Core/MemoryUtils.h"
#include "Core/StringUtils.h"

namespace Graphic
{
    Program::Program()
        : m_id( 0 ), m_type( 0 ), m_handle( 0 ), m_samplerCount( 0 )
    {
        MemoryUtils::MemSet( m_name, 0, sizeof(m_name) );
    }

    Program::Program( U32 id, U32 type, const Char * name )
        : m_id( id ), m_type( type ), m_handle( 0 ), m_samplerCount( 0 )
    {
        StringUtils::StrCpy( m_name, sizeof(m_name), name );
    }

    Program::~Program()
    {
        RenderDevice::DeleteProgram( m_handle );
    }
}
