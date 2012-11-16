#pragma once
#ifndef _GRAPHIC_OPENGL_H
#define _GRAPHIC_OPENGL_H

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Graphic/ps/win32/OpenGL.inl"
#else
    #error OpenGL not supported !
#endif

#endif // _GRAPHIC_OPENGL_H
