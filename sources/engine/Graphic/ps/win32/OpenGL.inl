
#if defined( CARBON_RETAIL )
#define GLAPI
#else
#define GLAPI __declspec(dllimport)
#endif

#include "GL/glcorearb.h"
#include "GL/wglext.h"

#ifdef __cplusplus
extern "C"
{
    // GL_VERSION_1_0
    GLAPI void              APIENTRY glCullFace (GLenum mode);
    GLAPI void              APIENTRY glFrontFace (GLenum mode);
    GLAPI void              APIENTRY glHint (GLenum target, GLenum mode);
    GLAPI void              APIENTRY glLineWidth (GLfloat width);
    GLAPI void              APIENTRY glPointSize (GLfloat size);
    GLAPI void              APIENTRY glPolygonMode (GLenum face, GLenum mode);
    GLAPI void              APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
    GLAPI void              APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
    GLAPI void              APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
    GLAPI void              APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
    GLAPI void              APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
    GLAPI void              APIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    GLAPI void              APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    GLAPI void              APIENTRY glDrawBuffer (GLenum mode);
    GLAPI void              APIENTRY glClear (GLbitfield mask);
    GLAPI void              APIENTRY glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    GLAPI void              APIENTRY glClearStencil (GLint s);
    GLAPI void              APIENTRY glClearDepth (GLdouble depth);
    GLAPI void              APIENTRY glStencilMask (GLuint mask);
    GLAPI void              APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    GLAPI void              APIENTRY glDepthMask (GLboolean flag);
    GLAPI void              APIENTRY glDisable (GLenum cap);
    GLAPI void              APIENTRY glEnable (GLenum cap);
    GLAPI void              APIENTRY glFinish (void);
    GLAPI void              APIENTRY glFlush (void);
    GLAPI void              APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
    GLAPI void              APIENTRY glLogicOp (GLenum opcode);
    GLAPI void              APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
    GLAPI void              APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
    GLAPI void              APIENTRY glDepthFunc (GLenum func);
    GLAPI void              APIENTRY glPixelStoref (GLenum pname, GLfloat param);
    GLAPI void              APIENTRY glPixelStorei (GLenum pname, GLint param);
    GLAPI void              APIENTRY glReadBuffer (GLenum mode);
    GLAPI void              APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
    GLAPI void              APIENTRY glGetBooleanv (GLenum pname, GLboolean *params);
    GLAPI void              APIENTRY glGetDoublev (GLenum pname, GLdouble *params);
    GLAPI GLenum            APIENTRY glGetError (void);
    GLAPI void              APIENTRY glGetFloatv (GLenum pname, GLfloat *params);
    GLAPI void              APIENTRY glGetIntegerv (GLenum pname, GLint *params);
    GLAPI const GLubyte *   APIENTRY glGetString (GLenum name);
    GLAPI void              APIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
    GLAPI void              APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
    GLAPI void              APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
    GLAPI void              APIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
    GLAPI void              APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
    GLAPI GLboolean         APIENTRY glIsEnabled (GLenum cap);
    GLAPI void              APIENTRY glDepthRange (GLdouble near, GLdouble far);
    GLAPI void              APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);

    // GL_VERSION_1_1
    GLAPI void              APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count);
    GLAPI void              APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
    GLAPI void              APIENTRY glGetPointerv (GLenum pname, GLvoid* *params);
    GLAPI void              APIENTRY glPolygonOffset (GLfloat factor, GLfloat units);
    GLAPI void              APIENTRY glCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
    GLAPI void              APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
    GLAPI void              APIENTRY glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
    GLAPI void              APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    GLAPI void              APIENTRY glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
    GLAPI void              APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    GLAPI void              APIENTRY glBindTexture (GLenum target, GLuint texture);
    GLAPI void              APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
    GLAPI void              APIENTRY glGenTextures (GLsizei n, GLuint *textures);
    GLAPI GLboolean         APIENTRY glIsTexture (GLuint texture);
}
#endif

#define META_GL_FUNCTIONS( Func )                                                   \
    /* GL_VERSION_1_3 */                                                            \
    Func ( PFNGLACTIVETEXTUREPROC               , glActiveTexture               );  \
    Func ( PFNGLSAMPLECOVERAGEPROC              , glSampleCoverage              );  \
    Func ( PFNGLCOMPRESSEDTEXIMAGE3DPROC        , glCompressedTexImage3D        );  \
    Func ( PFNGLCOMPRESSEDTEXIMAGE2DPROC        , glCompressedTexImage2D        );  \
    Func ( PFNGLCOMPRESSEDTEXIMAGE1DPROC        , glCompressedTexImage1D        );  \
    Func ( PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC     , glCompressedTexSubImage3D     );  \
    Func ( PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC     , glCompressedTexSubImage2D     );  \
    Func ( PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC     , glCompressedTexSubImage1D     );  \
    Func ( PFNGLGETCOMPRESSEDTEXIMAGEPROC       , glGetCompressedTexImage       );  \
                                                                                    \
    /* GL_VERSION_1_5 */                                                            \
    Func ( PFNGLBINDBUFFERPROC                  , glBindBuffer                  );  \
    Func ( PFNGLDELETEBUFFERSPROC               , glDeleteBuffers               );  \
    Func ( PFNGLGENBUFFERSPROC                  , glGenBuffers                  );  \
    Func ( PFNGLBUFFERDATAPROC                  , glBufferData                  );  \
    Func ( PFNGLBUFFERSUBDATAPROC               , glBufferSubData               );  \
                                                                                    \
    /* GL_VERSION_2_0 */                                                            \
    Func ( PFNGLATTACHSHADERPROC                , glAttachShader                );  \
    Func ( PFNGLCOMPILESHADERPROC               , glCompileShader               );  \
    Func ( PFNGLCREATEPROGRAMPROC               , glCreateProgram               );  \
    Func ( PFNGLCREATESHADERPROC                , glCreateShader                );  \
    Func ( PFNGLDELETEPROGRAMPROC               , glDeleteProgram               );  \
    Func ( PFNGLDELETESHADERPROC                , glDeleteShader                );  \
    Func ( PFNGLDETACHSHADERPROC                , glDetachShader                );  \
    Func ( PFNGLDISABLEVERTEXATTRIBARRAYPROC    , glDisableVertexAttribArray    );  \
    Func ( PFNGLENABLEVERTEXATTRIBARRAYPROC     , glEnableVertexAttribArray     );  \
    Func ( PFNGLGETPROGRAMIVPROC                , glGetProgramiv                );  \
    Func ( PFNGLGETPROGRAMINFOLOGPROC           , glGetProgramInfoLog           );  \
    Func ( PFNGLGETSHADERIVPROC                 , glGetShaderiv                 );  \
    Func ( PFNGLGETSHADERINFOLOGPROC            , glGetShaderInfoLog            );  \
    Func ( PFNGLISPROGRAMPROC                   , glIsProgram                   );  \
    Func ( PFNGLISSHADERPROC                    , glIsShader                    );  \
    Func ( PFNGLLINKPROGRAMPROC                 , glLinkProgram                 );  \
    Func ( PFNGLSHADERSOURCEPROC                , glShaderSource                );  \
    Func ( PFNGLUSEPROGRAMPROC                  , glUseProgram                  );  \
    Func ( PFNGLVERTEXATTRIBPOINTERPROC         , glVertexAttribPointer         );  \
                                                                                    \
    /* GL_ARB_framebuffer_object */                                                 \
    Func ( PFNGLGENERATEMIPMAPPROC              , glGenerateMipmap              );  \
                                                                                    \
    /* GL_ARB_vertex_array_object */                                                \
    Func ( PFNGLBINDVERTEXARRAYPROC             , glBindVertexArray             );  \
    Func ( PFNGLDELETEVERTEXARRAYSPROC          , glDeleteVertexArrays          );  \
    Func ( PFNGLGENVERTEXARRAYSPROC             , glGenVertexArrays             );  \
    Func ( PFNGLISVERTEXARRAYPROC               , glIsVertexArray               );  \
                                                                                    \
    /* GL_ARB_sampler_objects */                                                    \
    Func ( PFNGLGENSAMPLERSPROC                 , glGenSamplers                 );  \
    Func ( PFNGLDELETESAMPLERSPROC              , glDeleteSamplers              );  \
    Func ( PFNGLISSAMPLERPROC                   , glIsSampler                   );  \
    Func ( PFNGLBINDSAMPLERPROC                 , glBindSampler                 );  \
    Func ( PFNGLSAMPLERPARAMETERIPROC           , glSamplerParameteri           );  \
    Func ( PFNGLSAMPLERPARAMETERIVPROC          , glSamplerParameteriv          );  \
    Func ( PFNGLSAMPLERPARAMETERFPROC           , glSamplerParameterf           );  \
    Func ( PFNGLSAMPLERPARAMETERFVPROC          , glSamplerParameterfv          );  \
    Func ( PFNGLSAMPLERPARAMETERIIVPROC         , glSamplerParameterIiv         );  \
    Func ( PFNGLSAMPLERPARAMETERIUIVPROC        , glSamplerParameterIuiv        );  \
    Func ( PFNGLGETSAMPLERPARAMETERIVPROC       , glGetSamplerParameteriv       );  \
    Func ( PFNGLGETSAMPLERPARAMETERIIVPROC      , glGetSamplerParameterIiv      );  \
    Func ( PFNGLGETSAMPLERPARAMETERFVPROC       , glGetSamplerParameterfv       );  \
    Func ( PFNGLGETSAMPLERPARAMETERIUIVPROC     , glGetSamplerParameterIuiv     );  \
                                                                                    \
    /* GL_ARB_get_program_binary */                                                 \
    Func ( PFNGLGETPROGRAMBINARYPROC            , glGetProgramBinary            );  \
    Func ( PFNGLPROGRAMBINARYPROC               , glProgramBinary               );  \
    Func ( PFNGLPROGRAMPARAMETERIPROC           , glProgramParameteri           );  \
                                                                                    \
    /* GL_ARB_texture_storage */                                                    \
    Func ( PFNGLTEXSTORAGE1DPROC                , glTexStorage1D                );  \
    Func ( PFNGLTEXSTORAGE2DPROC                , glTexStorage2D                );  \
    Func ( PFNGLTEXSTORAGE3DPROC                , glTexStorage3D                );  \
    Func ( PFNGLTEXTURESTORAGE1DEXTPROC         , glTextureStorage1DEXT         );  \
    Func ( PFNGLTEXTURESTORAGE2DEXTPROC         , glTextureStorage2DEXT         );  \
    Func ( PFNGLTEXTURESTORAGE3DEXTPROC         , glTextureStorage3DEXT         )

#define DECLARE_GL_FUNCTION( PFunc, Name )     extern PFunc Name
#define DEFINE_GL_FUNCTION( PFunc, Name )      PFunc Name = 0
#define LOAD_GL_FUNCTION( PFunc, Name )        Name = (PFunc)wglGetProcAddress( #Name )
#define UNLOAD_GL_FUNCTION( PFunc, Name )      Name = 0

META_GL_FUNCTIONS( DECLARE_GL_FUNCTION );

#include "Graphic/DLL.h"

namespace Graphic
{
    class _GraphicExport OpenGL
    {
    public:
        static HGLRC CreateContext( HDC hDC, const int *attribList );
        static void DestroyContext( HGLRC renderContext );
    };
}
