#include "Timer.h"

#include "Core/TimeUtils.h"
#include "Core/StringUtils.h"
#include "Core/Trace.h"

namespace Core
{
    //====================================================================================
    // Timer
    //====================================================================================

    Timer::Timer( const Char * name )
    {
        StringUtils::StrCpy( m_name, sizeof(m_name), name ); 
    }

    void Timer::Begin()
    {
        m_elapsedTime = TimeUtils::ClockTime();
    }

    void Timer::End()
    {
        m_elapsedTime = TimeUtils::ClockTime() - m_elapsedTime;
    }

    U64 Timer::GetElapsedTimeInClockTick() const
    {
        return m_elapsedTime;
    }

    F64 Timer::GetElapsedTimeInSeconds() const
    {
        return (F64)m_elapsedTime * TimeUtils::ClockPeriod();
    }

    const Char * Timer::GetName() const
    {
        return m_name;
    }

    //====================================================================================
    // AutoTimer
    //====================================================================================

    AutoTimer::AutoTimer( const Char * name )
        : m_timer( name )
    {
        m_timer.Begin();
    }

    AutoTimer::~AutoTimer()
    {
        m_timer.End();

        Char buffer[128];
        CARBON_FORMAT_STRING( buffer, sizeof(buffer), "%s : %0.5f seconds\n", m_timer.GetName(), m_timer.GetElapsedTimeInSeconds() );

        CARBON_TRACE( buffer );
    }
}
