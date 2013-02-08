#pragma once
#ifndef _CORE_TRACE_H
#define _CORE_TRACE_H

#include "Core/Types.h"
#include "Core/DLL.h"

class _CoreExport Trace
{
public:
    static void Message( const Char * msg );
};

#define CARBON_TRACE( msg ) Trace::Message( msg )

#endif // _CORE_TRACE_H
