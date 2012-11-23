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
    GLAPI void              APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
    GLAPI void              APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);

    GLAPI void              APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    GLAPI void              APIENTRY glBindTexture (GLenum target, GLuint texture);
    GLAPI void              APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
    GLAPI void              APIENTRY glGenTextures (GLsizei n, GLuint *textures);
    GLAPI GLboolean         APIENTRY glIsTexture (GLuint texture);
}
#endif

extern PFNGLGENERATEMIPMAPPROC              glGenerateMipmap;

extern PFNGLCOMPRESSEDTEXIMAGE3DPROC        glCompressedTexImage3D;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC        glCompressedTexImage2D;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC        glCompressedTexImage1D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC     glCompressedTexSubImage3D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC     glCompressedTexSubImage2D;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC     glCompressedTexSubImage1D;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC       glGetCompressedTexImage;

extern PFNGLTEXSTORAGE1DPROC                glTexStorage1D;
extern PFNGLTEXSTORAGE2DPROC                glTexStorage2D;
extern PFNGLTEXSTORAGE3DPROC                glTexStorage3D;
extern PFNGLTEXTURESTORAGE1DEXTPROC         glTextureStorage1DEXT;
extern PFNGLTEXTURESTORAGE2DEXTPROC         glTextureStorage2DEXT;
extern PFNGLTEXTURESTORAGE3DEXTPROC         glTextureStorage3DEXT;

#endif

enum TextureProfile
{
    TP_RAW,
    TP_COLOR,
    TP_LINEAR,
    TP_NORMAL,
    TP_COUNT
};

bool CompileTexture( const char * inFilename, const char * outFilename, TextureProfile profile, bool mipMapGen );

#endif // _TEXTURECOMPILER_TEXTURECOMPILER_H
