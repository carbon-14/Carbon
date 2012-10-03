#pragma once
#ifndef _CORE_TIMER_H
#define _CORE_TIMER_H

#include "Core/Types.h"
#include "Core/DLL.h"

namespace Core
{
    class _CoreExport Timer
    {
    public:
        Timer( const Char * name );

        inline void         Begin();
        inline void         End();

        inline U64          GetElapsedTimeInClockTick() const;
        inline F64          GetElapsedTimeInSeconds() const;
        inline const Char * GetName() const;

    private:
        U64     m_elapsedTime;
        Char    m_name[64];
    };

    //====================================================================================

    class _CoreExport AutoTimer
    {
    public:
        AutoTimer( const Char * name );
        ~AutoTimer();

    private:
        Timer   m_timer;
    };
}

#define CARBON_AUTO_TIMER( id, name ) Core::AutoTimer id( name )

#endif // _CORE_TIMER_H
