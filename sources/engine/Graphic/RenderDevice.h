#pragma once
#ifndef _GRAPHIC_RENDERDEVICE_H
#define _GRAPHIC_RENDERDEVICE_H

#include "Core/Types.h"

namespace Graphic
{
    enum ShaderType
    {
        ST_VERTEX_SHADER = 0,
        ST_FRAGMENT_SHADER,
        ST_GEOMETRY_SHADER,
        ST_COUNT
    };

    enum DataType
    {
        DT_S8 = 0,
        DT_U8,
        DT_S16,
        DT_U16,
        DT_S32,
        DT_U32,
        DT_F32,
        DT_F64
    };

    const SizeT DataTypeSize[] = { 1, 1, 2, 2, 4, 4, 4, 8 };

    enum VertexSemantic
    {
        VS_POSITION = 0,
        VS_NORMAL,
        VS_TANGENT,
        VS_BINORMAL,
        VS_COLOR,
        VS_TEXCOORD0,
        VS_TEXCOORD1,
        VS_COUNT
    };

    enum VertexArrayUsage
    {
        VAU_STATIC = 0,
        VAU_DYNAMIC,
        VAU_STREAM
    };

    struct AttribDeclaration
    {
        VertexSemantic  m_semantic;
        DataType        m_type;
        SizeT           m_size;
        Bool            m_normalized;
        SizeT           m_offset;
    };

    enum PrimitiveType
    {
        PT_POINTS,
        PT_LINES,
        PT_TRIANGLES
    };

    enum TextureFilteringType
    {
        FT_POINT,
        FT_LINEAR
    };

    enum TextureWrapType
    {
        WT_REPEAT,
        WT_CLAMP,
        WT_MIRROR
    };

    struct VertexArray;

    typedef U32 Handle;
}

#if defined( CARBON_PLATFORM_WIN32 )
    #include "Graphic/ps/win32/RenderDevice.inl"
#else
    #error RenderDevice not implemented !
#endif

#endif // _GRAPHIC_RENDERDEVICE_H
