#pragma once
#ifndef _UNITTEST_APPLICATION_H
#define _UNITTEST_APPLICATION_H

#include "Core/Types.h"

#include "Graphic/RenderDevice.h"
#include "Graphic/ProgramCache.h"

#include <Windows.h>

struct RenderWindow
{
    SizeT       width;
    SizeT       height;

    HINSTANCE   hInstance;
    MSG         msg;
    HWND        hwnd;
    WNDCLASS    wc;
};

BOOL CreateRenderWindow( HINSTANCE hInstance, RenderWindow& window, SizeT width, SizeT height, bool fullscreen );
void DestroyRenderWindow( const RenderWindow& window );

class Application
{
public:
    Application( const RenderWindow& window );

    WPARAM Run();

protected:
    virtual void ProcessInputs( RAWINPUT * raw ) = 0;
    virtual void PreExecute() = 0;
    virtual void Execute() = 0;
    virtual void PostExecute() = 0;

private:
    Bool Initialize();
    void Destroy();
    Bool Update();

    Bool MessagePump( MSG * msg );
    void DisplayFramerate();

protected:
    RenderWindow            m_window;
    Graphic::RenderDevice   m_renderDevice;

    U64                     m_clockTicks;
    U64                     m_frameTicks;

    U64                     m_ticksCount;
    SizeT                   m_frameCount;
};

#endif // UNITTEST_APPLICATION_H
