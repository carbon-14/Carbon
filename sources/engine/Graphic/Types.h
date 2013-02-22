#pragma once
#ifndef _GRAPHIC_TYPES_H
#define _GRAPHIC_TYPES_H

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
        DT_F16,
        DT_S32,
        DT_U32,
        DT_F32,
        DT_F64,
        DT_S2_10_10_10,
        DT_U2_10_10_10
    };

    const SizeT DataTypeSize[] = { 1, 1, 2, 2, 2, 4, 4, 4, 8, 4, 4 };

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

    enum BufferUsage
    {
        BU_STATIC = 0,
        BU_DYNAMIC,
        BU_STREAM
    };

    enum BufferAccess
    {
        BA_READ_ONLY = 0,
        BA_WRITE_ONLY,
        BA_READ_WRITE
    };

    enum TextureFormat
    {
        TF_RGBA8 = 0,
        TF_RG16,
        TF_D24S8
    };

    enum FramebufferTarget
    {
        FT_READ = 0,
        FT_DRAW,
        FT_BOTH
    };

    enum FramebufferAttachment
    {
        FA_COLOR0 = 0,
        FA_COLOR1,
        FA_COLOR2,
        FA_COLOR3,
        FA_DEPTH,
        FA_STENCIL,
        FA_DEPTH_STENCIL
    };

    enum PrimitiveType
    {
        PT_POINTS = 0,
        PT_LINES,
        PT_TRIANGLES
    };

    enum FilterType
    {
        FT_POINT = 0,
        FT_LINEAR
    };

    enum MipType
    {
        MT_NONE = 0,
        MT_POINT,
        MT_LINEAR
    };

    enum WrapType
    {
        WT_REPEAT = 0,
        WT_CLAMP,
        WT_MIRROR
    };

    enum ClearMode
    {
        CM_COLOR            = 1 << 0,
        CM_DEPTH            = 1 << 1,
        CM_STENCIL          = 1 << 2
    };

    enum ColorMode
    {
        CM_RED      = 1 << 0,
        CM_GREEN    = 1 << 1,
        CM_BLUE     = 1 << 2,
        CM_ALPHA    = 1 << 3,
        CM_RGB      = CM_RED | CM_GREEN | CM_BLUE,
        CM_RGBA     = CM_RGB | CM_ALPHA
    };

    enum Function
    {
        F_NEVER,
        F_LESS,
        F_EQUAL,
        F_LEQUAL,
        F_GREATER,
        F_NOTEQUAL,
        F_GEQUAL,
        F_ALWAYS
    };

    enum Operation
    {
        O_KEEP,
        O_ZERO,
        O_REPLACE,
        O_INCR,
        O_INCR_WRAP,
        O_DECR,
        O_DECR_WRAP,
        O_INVERT
    };

    enum CullFace
    {
        CF_FRONT,
        CF_BACK
    };

    enum BlendFunction
    {
        BF_ZERO,
        BF_ONE,
        BF_SRC_COLOR,
        BF_ONE_MINUS_SRC_COLOR,
        BF_DST_COLOR,
        BF_ONE_MINUS_DST_COLOR,
        BF_SRC_ALPHA,
        BF_ONE_MINUS_SRC_ALPHA,
        BF_DST_ALPHA,
        BF_ONE_MINUS_DST_ALPHA,
        BF_CONSTANT_COLOR,
        BF_ONE_MINUS_CONSTANT_COLOR,
        BF_CONSTANT_ALPHA,
        BF_ONE_MINUS_CONSTANT_ALPHA,
        BF_SRC_ALPHA_SATURATE,
        BF_SRC1_COLOR,
        BF_ONE_MINUS_SRC1_COLOR,
        BF_SRC1_ALPHA,
        BF_ONE_MINUS_SRC1_ALPHA
    };

    enum BlendMode
    {
        BM_ADD,
        BM_SUBSTRACT,
        BM_REVERSE_SUBSTRACT,
        BM_MIN,
        BM_MAX
    };

    typedef U32 Handle;

    struct LayoutHandle
    {
        Handle  m_handle;
        SizeT   m_index;
    };

    enum LayoutIndex
    {
        LI_SCENE    = 5,
        LI_INSTANCE = 6,
        LI_MATERIAL = 7
    };

    struct AttribDeclaration
    {
        VertexSemantic  m_semantic;
        DataType        m_type;
        SizeT           m_size;
        Bool            m_normalized;
        SizeT           m_offset;
    };

    const SizeT s_maxVertexAttribCount = 16;
    struct VertexDeclaration
    {
        AttribDeclaration   m_attributes[ s_maxVertexAttribCount ];
        SizeT               m_size;
        SizeT               m_count;
    };

    const SizeT s_maxTextureUnitCount       = 8;
    const SizeT s_maxUniformBufferCount     = 8;
}

#endif // _GRAPHIC_RENDERTYPES_H