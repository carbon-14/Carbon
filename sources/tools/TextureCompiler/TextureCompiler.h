#pragma once
#ifndef _TEXTURECOMPILER_TEXTURECOMPILER_H
#define _TEXTURECOMPILER_TEXTURECOMPILER_H

#if defined( CARBON_PLATFORM_WIN32 )
    #define GLAPI __declspec(dllimport)
#endif

#include "GL/glcorearb.h"

#if defined( CARBON_PLATFORM_WIN32 )
#include "GL/wglext.h"

#ifdef __cplusplus
extern "C"
{
    GLAPI void              APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    GLAPI void              APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);

    GLAPI void              APIENTRY glBindTexture (GLenum target, GLuint texture);
    GLAPI void              APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
    GLAPI void              APIENTRY glGenTextures (GLsizei n, GLuint *textures);
    GLAPI GLboolean         APIENTRY glIsTexture (GLuint texture);
}
#endif

extern PFNGLCOMPRESSEDTEXIMAGE3DPROC        glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC        glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC        glCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC     glCompressedTexSubImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC     glCompressedTexSubImage2D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC     glCompressedTexSubImage1D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC       glGetCompressedTexImage;

#endif

enum CompressionFormat
{
    CF_UNCOMPRESSED,
    CF_BC1,
    CF_BC2,
    CF_BC3,
    CF_BC4,
    CF_BC5,
    CF_BC6,
    CF_BC7
};

bool CompileTexture( const char * inFilename, const char * outFilename, CompressionFormat compression );

#endif // _TEXTURECOMPILER_TEXTURECOMPILER_H
