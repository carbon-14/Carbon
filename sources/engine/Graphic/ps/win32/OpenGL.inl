
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

#define META_GL_FUNCTIONS( Func )                                                                                   \
    /* GL_VERSION_1_2 */                                                                                            \
    Func ( PFNGLBLENDCOLORPROC                                  , glBlendColor                                  );  \
    Func ( PFNGLBLENDEQUATIONPROC                               , glBlendEquation                               );  \
    Func ( PFNGLDRAWRANGEELEMENTSPROC                           , glDrawRangeElements                           );  \
    Func ( PFNGLTEXIMAGE3DPROC                                  , glTexImage3D                                  );  \
    Func ( PFNGLTEXSUBIMAGE3DPROC                               , glTexSubImage3D                               );  \
    Func ( PFNGLCOPYTEXSUBIMAGE3DPROC                           , glCopyTexSubImage3D                           );  \
                                                                                                                    \
    /* GL_VERSION_1_3 */                                                                                            \
    Func ( PFNGLACTIVETEXTUREPROC                               , glActiveTexture                               );  \
    Func ( PFNGLSAMPLECOVERAGEPROC                              , glSampleCoverage                              );  \
    Func ( PFNGLCOMPRESSEDTEXIMAGE3DPROC                        , glCompressedTexImage3D                        );  \
    Func ( PFNGLCOMPRESSEDTEXIMAGE2DPROC                        , glCompressedTexImage2D                        );  \
    Func ( PFNGLCOMPRESSEDTEXIMAGE1DPROC                        , glCompressedTexImage1D                        );  \
    Func ( PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC                     , glCompressedTexSubImage3D                     );  \
    Func ( PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC                     , glCompressedTexSubImage2D                     );  \
    Func ( PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC                     , glCompressedTexSubImage1D                     );  \
    Func ( PFNGLGETCOMPRESSEDTEXIMAGEPROC                       , glGetCompressedTexImage                       );  \
                                                                                                                    \
    /* GL_VERSION_1_4 */                                                                                            \
    Func ( PFNGLBLENDFUNCSEPARATEPROC                           , glBlendFuncSeparate                           );  \
    Func ( PFNGLMULTIDRAWARRAYSPROC                             , glMultiDrawArrays                             );  \
    Func ( PFNGLMULTIDRAWELEMENTSPROC                           , glMultiDrawElements                           );  \
    Func ( PFNGLPOINTPARAMETERFPROC                             , glPointParameterf                             );  \
    Func ( PFNGLPOINTPARAMETERFVPROC                            , glPointParameterfv                            );  \
    Func ( PFNGLPOINTPARAMETERIPROC                             , glPointParameteri                             );  \
    Func ( PFNGLPOINTPARAMETERIVPROC                            , glPointParameteriv                            );  \
                                                                                                                    \
    /* GL_VERSION_1_5 */                                                                                            \
    Func ( PFNGLGENQUERIESPROC                                  , glGenQueries                                  );  \
    Func ( PFNGLDELETEQUERIESPROC                               , glDeleteQueries                               );  \
    Func ( PFNGLISQUERYPROC                                     , glIsQuery                                     );  \
    Func ( PFNGLBEGINQUERYPROC                                  , glBeginQuery                                  );  \
    Func ( PFNGLENDQUERYPROC                                    , glEndQuery                                    );  \
    Func ( PFNGLGETQUERYIVPROC                                  , glGetQueryiv                                  );  \
    Func ( PFNGLGETQUERYOBJECTIVPROC                            , glGetQueryObjectiv                            );  \
    Func ( PFNGLGETQUERYOBJECTUIVPROC                           , glGetQueryObjectuiv                           );  \
    Func ( PFNGLBINDBUFFERPROC                                  , glBindBuffer                                  );  \
    Func ( PFNGLDELETEBUFFERSPROC                               , glDeleteBuffers                               );  \
    Func ( PFNGLGENBUFFERSPROC                                  , glGenBuffers                                  );  \
    Func ( PFNGLISBUFFERPROC                                    , glIsBuffer                                    );  \
    Func ( PFNGLBUFFERDATAPROC                                  , glBufferData                                  );  \
    Func ( PFNGLBUFFERSUBDATAPROC                               , glBufferSubData                               );  \
    Func ( PFNGLGETBUFFERSUBDATAPROC                            , glGetBufferSubData                            );  \
    Func ( PFNGLMAPBUFFERPROC                                   , glMapBuffer                                   );  \
    Func ( PFNGLUNMAPBUFFERPROC                                 , glUnmapBuffer                                 );  \
    Func ( PFNGLGETBUFFERPARAMETERIVPROC                        , glGetBufferParameteriv                        );  \
    Func ( PFNGLGETBUFFERPOINTERVPROC                           , glGetBufferPointerv                           );  \
                                                                                                                    \
    /* GL_VERSION_2_0 */                                                                                            \
    Func ( PFNGLBLENDEQUATIONSEPARATEPROC                       , glBlendEquationSeparate                       );  \
    Func ( PFNGLDRAWBUFFERSPROC                                 , glDrawBuffers                                 );  \
    Func ( PFNGLSTENCILOPSEPARATEPROC                           , glStencilOpSeparate                           );  \
    Func ( PFNGLSTENCILFUNCSEPARATEPROC                         , glStencilFuncSeparate                         );  \
    Func ( PFNGLSTENCILMASKSEPARATEPROC                         , glStencilMaskSeparate                         );  \
    Func ( PFNGLATTACHSHADERPROC                                , glAttachShader                                );  \
    Func ( PFNGLBINDATTRIBLOCATIONPROC                          , glBindAttribLocation                          );  \
    Func ( PFNGLCOMPILESHADERPROC                               , glCompileShader                               );  \
    Func ( PFNGLCREATEPROGRAMPROC                               , glCreateProgram                               );  \
    Func ( PFNGLCREATESHADERPROC                                , glCreateShader                                );  \
    Func ( PFNGLDELETEPROGRAMPROC                               , glDeleteProgram                               );  \
    Func ( PFNGLDELETESHADERPROC                                , glDeleteShader                                );  \
    Func ( PFNGLDETACHSHADERPROC                                , glDetachShader                                );  \
    Func ( PFNGLDISABLEVERTEXATTRIBARRAYPROC                    , glDisableVertexAttribArray                    );  \
    Func ( PFNGLENABLEVERTEXATTRIBARRAYPROC                     , glEnableVertexAttribArray                     );  \
    Func ( PFNGLGETACTIVEATTRIBPROC                             , glGetActiveAttrib                             );  \
    Func ( PFNGLGETACTIVEUNIFORMPROC                            , glGetActiveUniform                            );  \
    Func ( PFNGLGETATTACHEDSHADERSPROC                          , glGetAttachedShaders                          );  \
    Func ( PFNGLGETATTRIBLOCATIONPROC                           , glGetAttribLocation                           );  \
    Func ( PFNGLGETPROGRAMIVPROC                                , glGetProgramiv                                );  \
    Func ( PFNGLGETPROGRAMINFOLOGPROC                           , glGetProgramInfoLog                           );  \
    Func ( PFNGLGETSHADERIVPROC                                 , glGetShaderiv                                 );  \
    Func ( PFNGLGETSHADERINFOLOGPROC                            , glGetShaderInfoLog                            );  \
    Func ( PFNGLGETSHADERSOURCEPROC                             , glGetShaderSource                             );  \
    Func ( PFNGLGETUNIFORMLOCATIONPROC                          , glGetUniformLocation                          );  \
    Func ( PFNGLGETUNIFORMFVPROC                                , glGetUniformfv                                );  \
    Func ( PFNGLGETUNIFORMIVPROC                                , glGetUniformiv                                );  \
    Func ( PFNGLGETVERTEXATTRIBDVPROC                           , glGetVertexAttribdv                           );  \
    Func ( PFNGLGETVERTEXATTRIBFVPROC                           , glGetVertexAttribfv                           );  \
    Func ( PFNGLGETVERTEXATTRIBIVPROC                           , glGetVertexAttribiv                           );  \
    Func ( PFNGLGETVERTEXATTRIBPOINTERVPROC                     , glGetVertexAttribPointerv                     );  \
    Func ( PFNGLISPROGRAMPROC                                   , glIsProgram                                   );  \
    Func ( PFNGLISSHADERPROC                                    , glIsShader                                    );  \
    Func ( PFNGLLINKPROGRAMPROC                                 , glLinkProgram                                 );  \
    Func ( PFNGLSHADERSOURCEPROC                                , glShaderSource                                );  \
    Func ( PFNGLUSEPROGRAMPROC                                  , glUseProgram                                  );  \
    Func ( PFNGLUNIFORM1FPROC                                   , glUniform1f                                   );  \
    Func ( PFNGLUNIFORM2FPROC                                   , glUniform2f                                   );  \
    Func ( PFNGLUNIFORM3FPROC                                   , glUniform3f                                   );  \
    Func ( PFNGLUNIFORM4FPROC                                   , glUniform4f                                   );  \
    Func ( PFNGLUNIFORM1IPROC                                   , glUniform1i                                   );  \
    Func ( PFNGLUNIFORM2IPROC                                   , glUniform2i                                   );  \
    Func ( PFNGLUNIFORM3IPROC                                   , glUniform3i                                   );  \
    Func ( PFNGLUNIFORM4IPROC                                   , glUniform4i                                   );  \
    Func ( PFNGLUNIFORM1FVPROC                                  , glUniform1fv                                  );  \
    Func ( PFNGLUNIFORM2FVPROC                                  , glUniform2fv                                  );  \
    Func ( PFNGLUNIFORM3FVPROC                                  , glUniform3fv                                  );  \
    Func ( PFNGLUNIFORM4FVPROC                                  , glUniform4fv                                  );  \
    Func ( PFNGLUNIFORM1IVPROC                                  , glUniform1iv                                  );  \
    Func ( PFNGLUNIFORM2IVPROC                                  , glUniform2iv                                  );  \
    Func ( PFNGLUNIFORM3IVPROC                                  , glUniform3iv                                  );  \
    Func ( PFNGLUNIFORM4IVPROC                                  , glUniform4iv                                  );  \
    Func ( PFNGLUNIFORMMATRIX2FVPROC                            , glUniformMatrix2fv                            );  \
    Func ( PFNGLUNIFORMMATRIX3FVPROC                            , glUniformMatrix3fv                            );  \
    Func ( PFNGLUNIFORMMATRIX4FVPROC                            , glUniformMatrix4fv                            );  \
    Func ( PFNGLVALIDATEPROGRAMPROC                             , glValidateProgram                             );  \
    Func ( PFNGLVERTEXATTRIB1DPROC                              , glVertexAttrib1d                              );  \
    Func ( PFNGLVERTEXATTRIB1DVPROC                             , glVertexAttrib1dv                             );  \
    Func ( PFNGLVERTEXATTRIB1FPROC                              , glVertexAttrib1f                              );  \
    Func ( PFNGLVERTEXATTRIB1FVPROC                             , glVertexAttrib1fv                             );  \
    Func ( PFNGLVERTEXATTRIB1SPROC                              , glVertexAttrib1s                              );  \
    Func ( PFNGLVERTEXATTRIB1SVPROC                             , glVertexAttrib1sv                             );  \
    Func ( PFNGLVERTEXATTRIB2DPROC                              , glVertexAttrib2d                              );  \
    Func ( PFNGLVERTEXATTRIB2DVPROC                             , glVertexAttrib2dv                             );  \
    Func ( PFNGLVERTEXATTRIB2FPROC                              , glVertexAttrib2f                              );  \
    Func ( PFNGLVERTEXATTRIB2FVPROC                             , glVertexAttrib2fv                             );  \
    Func ( PFNGLVERTEXATTRIB2SPROC                              , glVertexAttrib2s                              );  \
    Func ( PFNGLVERTEXATTRIB2SVPROC                             , glVertexAttrib2sv                             );  \
    Func ( PFNGLVERTEXATTRIB3DPROC                              , vglVertexAttrib3d                             );  \
    Func ( PFNGLVERTEXATTRIB3DVPROC                             , glVertexAttrib3dv                             );  \
    Func ( PFNGLVERTEXATTRIB3FPROC                              , glVertexAttrib3f                              );  \
    Func ( PFNGLVERTEXATTRIB3FVPROC                             , glVertexAttrib3fv                             );  \
    Func ( PFNGLVERTEXATTRIB3SPROC                              , glVertexAttrib3s                              );  \
    Func ( PFNGLVERTEXATTRIB3SVPROC                             , glVertexAttrib3sv                             );  \
    Func ( PFNGLVERTEXATTRIB4NBVPROC                            , glVertexAttrib4Nbv                            );  \
    Func ( PFNGLVERTEXATTRIB4NIVPROC                            , glVertexAttrib4Niv                            );  \
    Func ( PFNGLVERTEXATTRIB4NSVPROC                            , glVertexAttrib4Nsv                            );  \
    Func ( PFNGLVERTEXATTRIB4NUBPROC                            , glVertexAttrib4Nub                            );  \
    Func ( PFNGLVERTEXATTRIB4NUBVPROC                           , glVertexAttrib4Nubv                           );  \
    Func ( PFNGLVERTEXATTRIB4NUIVPROC                           , glVertexAttrib4Nuiv                           );  \
    Func ( PFNGLVERTEXATTRIB4NUSVPROC                           , glVertexAttrib4Nusv                           );  \
    Func ( PFNGLVERTEXATTRIB4BVPROC                             , glVertexAttrib4bv                             );  \
    Func ( PFNGLVERTEXATTRIB4DPROC                              , glVertexAttrib4d                              );  \
    Func ( PFNGLVERTEXATTRIB4DVPROC                             , glVertexAttrib4dv                             );  \
    Func ( PFNGLVERTEXATTRIB4FPROC                              , glVertexAttrib4f                              );  \
    Func ( PFNGLVERTEXATTRIB4FVPROC                             , glVertexAttrib4fv                             );  \
    Func ( PFNGLVERTEXATTRIB4IVPROC                             , glVertexAttrib4iv                             );  \
    Func ( PFNGLVERTEXATTRIB4SPROC                              , glVertexAttrib4s                              );  \
    Func ( PFNGLVERTEXATTRIB4SVPROC                             , glVertexAttrib4sv                             );  \
    Func ( PFNGLVERTEXATTRIB4UBVPROC                            , glVertexAttrib4ubv                            );  \
    Func ( PFNGLVERTEXATTRIB4UIVPROC                            , glVertexAttrib4uiv                            );  \
    Func ( PFNGLVERTEXATTRIB4USVPROC                            , glVertexAttrib4usv                            );  \
    Func ( PFNGLVERTEXATTRIBPOINTERPROC                         , glVertexAttribPointer                         );  \
                                                                                                                    \
    /* GL_VERSION_2_1 */                                                                                            \
    Func ( PFNGLUNIFORMMATRIX2X3FVPROC                          , glUniformMatrix2x3fv                          );  \
    Func ( PFNGLUNIFORMMATRIX3X2FVPROC                          , glUniformMatrix3x2fv                          );  \
    Func ( PFNGLUNIFORMMATRIX2X4FVPROC                          , glUniformMatrix2x4fv                          );  \
    Func ( PFNGLUNIFORMMATRIX4X2FVPROC                          , glUniformMatrix4x2fv                          );  \
    Func ( PFNGLUNIFORMMATRIX3X4FVPROC                          , glUniformMatrix3x4fv                          );  \
    Func ( PFNGLUNIFORMMATRIX4X3FVPROC                          , glUniformMatrix4x3fv                          );  \
                                                                                                                    \
    /* GL_VERSION_3_0 */                                                                                            \
    Func ( PFNGLCOLORMASKIPROC                                  , glColorMaski                                  );  \
    Func ( PFNGLGETBOOLEANI_VPROC                               , glGetBooleani_v                               );  \
    Func ( PFNGLGETINTEGERI_VPROC                               , glGetIntegeri_v                               );  \
    Func ( PFNGLENABLEIPROC                                     , glEnablei                                     );  \
    Func ( PFNGLDISABLEIPROC                                    , glDisablei                                    );  \
    Func ( PFNGLISENABLEDIPROC                                  , glIsEnabledi                                  );  \
    Func ( PFNGLBEGINTRANSFORMFEEDBACKPROC                      , glBeginTransformFeedback                      );  \
    Func ( PFNGLENDTRANSFORMFEEDBACKPROC                        , glEndTransformFeedback                        );  \
    Func ( PFNGLBINDBUFFERRANGEPROC                             , glBindBufferRange                             );  \
    Func ( PFNGLBINDBUFFERBASEPROC                              , glBindBufferBase                              );  \
    Func ( PFNGLTRANSFORMFEEDBACKVARYINGSPROC                   , glTransformFeedbackVaryings                   );  \
    Func ( PFNGLGETTRANSFORMFEEDBACKVARYINGPROC                 , glGetTransformFeedbackVarying                 );  \
    Func ( PFNGLCLAMPCOLORPROC                                  , glClampColor                                  );  \
    Func ( PFNGLBEGINCONDITIONALRENDERPROC                      , glBeginConditionalRender                      );  \
    Func ( PFNGLENDCONDITIONALRENDERPROC                        , glEndConditionalRender                        );  \
    Func ( PFNGLVERTEXATTRIBIPOINTERPROC                        , glVertexAttribIPointer                        );  \
    Func ( PFNGLGETVERTEXATTRIBIIVPROC                          , glGetVertexAttribIiv                          );  \
    Func ( PFNGLGETVERTEXATTRIBIUIVPROC                         , glGetVertexAttribIuiv                         );  \
    Func ( PFNGLVERTEXATTRIBI1IPROC                             , glVertexAttribI1i                             );  \
    Func ( PFNGLVERTEXATTRIBI2IPROC                             , glVertexAttribI2i                             );  \
    Func ( PFNGLVERTEXATTRIBI3IPROC                             , glVertexAttribI3i                             );  \
    Func ( PFNGLVERTEXATTRIBI4IPROC                             , glVertexAttribI4i                             );  \
    Func ( PFNGLVERTEXATTRIBI1UIPROC                            , glVertexAttribI1ui                            );  \
    Func ( PFNGLVERTEXATTRIBI2UIPROC                            , glVertexAttribI2ui                            );  \
    Func ( PFNGLVERTEXATTRIBI3UIPROC                            , glVertexAttribI3ui                            );  \
    Func ( PFNGLVERTEXATTRIBI4UIPROC                            , glVertexAttribI4ui                            );  \
    Func ( PFNGLVERTEXATTRIBI1IVPROC                            , glVertexAttribI1iv                            );  \
    Func ( PFNGLVERTEXATTRIBI2IVPROC                            , glVertexAttribI2iv                            );  \
    Func ( PFNGLVERTEXATTRIBI3IVPROC                            , glVertexAttribI3iv                            );  \
    Func ( PFNGLVERTEXATTRIBI4IVPROC                            , glVertexAttribI4iv                            );  \
    Func ( PFNGLVERTEXATTRIBI1UIVPROC                           , glVertexAttribI1uiv                           );  \
    Func ( PFNGLVERTEXATTRIBI2UIVPROC                           , glVertexAttribI2uiv                           );  \
    Func ( PFNGLVERTEXATTRIBI3UIVPROC                           , glVertexAttribI3uiv                           );  \
    Func ( PFNGLVERTEXATTRIBI4UIVPROC                           , glVertexAttribI4uiv                           );  \
    Func ( PFNGLVERTEXATTRIBI4BVPROC                            , glVertexAttribI4ubv                           );  \
    Func ( PFNGLVERTEXATTRIBI4SVPROC                            , glVertexAttribI4usv                           );  \
    Func ( PFNGLVERTEXATTRIBI4UBVPROC                           , glVertexAttribI1ubv                           );  \
    Func ( PFNGLVERTEXATTRIBI4USVPROC                           , glVertexAttribI1usv                           );  \
    Func ( PFNGLGETUNIFORMUIVPROC                               , glGetUniformuiv                               );  \
    Func ( PFNGLBINDFRAGDATALOCATIONPROC                        , glBindFragDataLocation                        );  \
    Func ( PFNGLGETFRAGDATALOCATIONPROC                         , glGetFragDataLocation                         );  \
    Func ( PFNGLUNIFORM1UIPROC                                  , glUniform1ui                                  );  \
    Func ( PFNGLUNIFORM2UIPROC                                  , glUniform2ui                                  );  \
    Func ( PFNGLUNIFORM3UIPROC                                  , glUniform3ui                                  );  \
    Func ( PFNGLUNIFORM4UIPROC                                  , glUniform4ui                                  );  \
    Func ( PFNGLUNIFORM1UIVPROC                                 , glUniform1uiv                                 );  \
    Func ( PFNGLUNIFORM2UIVPROC                                 , glUniform2uiv                                 );  \
    Func ( PFNGLUNIFORM3UIVPROC                                 , glUniform3uiv                                 );  \
    Func ( PFNGLUNIFORM4UIVPROC                                 , glUniform4uiv                                 );  \
    Func ( PFNGLTEXPARAMETERIIVPROC                             , glTexParameterIiv                             );  \
    Func ( PFNGLTEXPARAMETERIUIVPROC                            , glTexParameterIuiv                            );  \
    Func ( PFNGLGETTEXPARAMETERIIVPROC                          , glGetTexParameterIiv                          );  \
    Func ( PFNGLGETTEXPARAMETERIUIVPROC                         , glGetTexParameterIuiv                         );  \
    Func ( PFNGLCLEARBUFFERIVPROC                               , glClearBufferiv                               );  \
    Func ( PFNGLCLEARBUFFERUIVPROC                              , glClearBufferuiv                              );  \
    Func ( PFNGLCLEARBUFFERFVPROC                               , glClearBufferfv                               );  \
    Func ( PFNGLCLEARBUFFERFIPROC                               , glClearBufferfi                               );  \
    Func ( PFNGLGETSTRINGIPROC                                  , glGetStringi                                  );  \
    Func ( PFNGLISRENDERBUFFERPROC                              , glIsRenderbuffer                              );  \
    Func ( PFNGLBINDRENDERBUFFERPROC                            , glBindRenderbuffer                            );  \
    Func ( PFNGLDELETERENDERBUFFERSPROC                         , glDeleteRenderbuffers                         );  \
    Func ( PFNGLGENRENDERBUFFERSPROC                            , glGenRenderbuffers                            );  \
    Func ( PFNGLRENDERBUFFERSTORAGEPROC                         , glRenderbufferStorage                         );  \
    Func ( PFNGLGETRENDERBUFFERPARAMETERIVPROC                  , glGetRenderbufferParameteriv                  );  \
    Func ( PFNGLISFRAMEBUFFERPROC                               , glIsFramebuffer                               );  \
    Func ( PFNGLBINDFRAMEBUFFERPROC                             , glBindFramebuffer                             );  \
    Func ( PFNGLDELETEFRAMEBUFFERSPROC                          , glDeleteFramebuffers                          );  \
    Func ( PFNGLGENFRAMEBUFFERSPROC                             , glGenFramebuffers                             );  \
    Func ( PFNGLCHECKFRAMEBUFFERSTATUSPROC                      , glCheckFramebufferStatus                      );  \
    Func ( PFNGLFRAMEBUFFERTEXTURE1DPROC                        , glFramebufferTexture1D                        );  \
    Func ( PFNGLFRAMEBUFFERTEXTURE2DPROC                        , glFramebufferTexture2D                        );  \
    Func ( PFNGLFRAMEBUFFERTEXTURE3DPROC                        , glFramebufferTexture3D                        );  \
    Func ( PFNGLFRAMEBUFFERRENDERBUFFERPROC                     , glFramebufferRenderbuffer                     );  \
    Func ( PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC         , glGetFramebufferAttachmentParameteriv         );  \
    Func ( PFNGLGENERATEMIPMAPPROC                              , glGenerateMipmap                              );  \
    Func ( PFNGLBLITFRAMEBUFFERPROC                             , glBlitFramebuffer                             );  \
    Func ( PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC              , glRenderbufferStorageMultisample              );  \
    Func ( PFNGLFRAMEBUFFERTEXTURELAYERPROC                     , glFramebufferTextureLayer                     );  \
    Func ( PFNGLMAPBUFFERRANGEPROC                              , glMapBufferRange                              );  \
    Func ( PFNGLFLUSHMAPPEDBUFFERRANGEPROC                      , glFlushMappedBufferRange                      );  \
    Func ( PFNGLBINDVERTEXARRAYPROC                             , glBindVertexArray                             );  \
    Func ( PFNGLDELETEVERTEXARRAYSPROC                          , glDeleteVertexArrays                          );  \
    Func ( PFNGLGENVERTEXARRAYSPROC                             , glGenVertexArrays                             );  \
    Func ( PFNGLISVERTEXARRAYPROC                               , glIsVertexArray                               );  \
                                                                                                                    \
    /* GL_VERSION_3_1 */                                                                                            \
    Func ( PFNGLDRAWARRAYSINSTANCEDPROC                         , glDrawArraysInstanced                         );  \
    Func ( PFNGLDRAWELEMENTSINSTANCEDPROC                       , glDrawElementsInstanced                       );  \
    Func ( PFNGLTEXBUFFERPROC                                   , glTexBuffer                                   );  \
    Func ( PFNGLPRIMITIVERESTARTINDEXPROC                       , glPrimitiveRestartIndex                       );  \
    Func ( PFNGLCOPYBUFFERSUBDATAPROC                           , glCopyBufferSubData                           );  \
    Func ( PFNGLGETUNIFORMINDICESPROC                           , glGetUniformIndices                           );  \
    Func ( PFNGLGETACTIVEUNIFORMSIVPROC                         , glGetActiveUniformsiv                         );  \
    Func ( PFNGLGETACTIVEUNIFORMNAMEPROC                        , glGetActiveUniformName                        );  \
    Func ( PFNGLGETUNIFORMBLOCKINDEXPROC                        , glGetUniformBlockIndex                        );  \
    Func ( PFNGLGETACTIVEUNIFORMBLOCKIVPROC                     , glGetActiveUniformBlockiv                     );  \
    Func ( PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC                   , glGetActiveUniformBlockName                   );  \
    Func ( PFNGLUNIFORMBLOCKBINDINGPROC                         , glUniformBlockBinding                         );  \
                                                                                                                    \
    /* GL_VERSION_3_2 */                                                                                            \
    Func ( PFNGLDRAWELEMENTSBASEVERTEXPROC                      , glDrawElementsBaseVertex                      );  \
    Func ( PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC                 , glDrawRangeElementsBaseVertex                 );  \
    Func ( PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC             , glDrawElementsInstancedBaseVertex             );  \
    Func ( PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC                 , glMultiDrawElementsBaseVertex                 );  \
    Func ( PFNGLPROVOKINGVERTEXPROC                             , glProvokingVertex                             );  \
    Func ( PFNGLFENCESYNCPROC                                   , glFenceSync                                   );  \
    Func ( PFNGLISSYNCPROC                                      , glIsSync                                      );  \
    Func ( PFNGLDELETESYNCPROC                                  , glDeleteSync                                  );  \
    Func ( PFNGLCLIENTWAITSYNCPROC                              , glClientWaitSync                              );  \
    Func ( PFNGLWAITSYNCPROC                                    , glWaitSync                                    );  \
    Func ( PFNGLGETINTEGER64VPROC                               , glGetInteger64v                               );  \
    Func ( PFNGLGETSYNCIVPROC                                   , glGetSynciv                                   );  \
    Func ( PFNGLGETINTEGER64I_VPROC                             , glGetInteger64i_v                             );  \
    Func ( PFNGLGETBUFFERPARAMETERI64VPROC                      , glGetBufferParameteri64v                      );  \
    Func ( PFNGLFRAMEBUFFERTEXTUREPROC                          , glFramebufferTexture                          );  \
    Func ( PFNGLTEXIMAGE2DMULTISAMPLEPROC                       , glTexImage2DMultisample                       );  \
    Func ( PFNGLTEXIMAGE3DMULTISAMPLEPROC                       , glTexImage3DMultisample                       );  \
    Func ( PFNGLGETMULTISAMPLEFVPROC                            , glGetMultisamplefv                            );  \
    Func ( PFNGLSAMPLEMASKIPROC                                 , glSampleMaski                                 );  \
                                                                                                                    \
    /* GL_VERSION_3_3 */                                                                                            \
    Func ( PFNGLBINDFRAGDATALOCATIONINDEXEDPROC                 , glBindFragDataLocationIndexed                 );  \
    Func ( PFNGLGETFRAGDATAINDEXPROC                            , glGetFragDataIndex                            );  \
    Func ( PFNGLGENSAMPLERSPROC                                 , glGenSamplers                                 );  \
    Func ( PFNGLDELETESAMPLERSPROC                              , glDeleteSamplers                              );  \
    Func ( PFNGLISSAMPLERPROC                                   , glIsSampler                                   );  \
    Func ( PFNGLBINDSAMPLERPROC                                 , glBindSampler                                 );  \
    Func ( PFNGLSAMPLERPARAMETERIPROC                           , glSamplerParameteri                           );  \
    Func ( PFNGLSAMPLERPARAMETERIVPROC                          , glSamplerParameteriv                          );  \
    Func ( PFNGLSAMPLERPARAMETERFPROC                           , glSamplerParameterf                           );  \
    Func ( PFNGLSAMPLERPARAMETERFVPROC                          , glSamplerParameterfv                          );  \
    Func ( PFNGLSAMPLERPARAMETERIIVPROC                         , glSamplerParameterIiv                         );  \
    Func ( PFNGLSAMPLERPARAMETERIUIVPROC                        , glSamplerParameterIuiv                        );  \
    Func ( PFNGLGETSAMPLERPARAMETERIVPROC                       , glGetSamplerParameteriv                       );  \
    Func ( PFNGLGETSAMPLERPARAMETERIIVPROC                      , glGetSamplerParameterIiv                      );  \
    Func ( PFNGLGETSAMPLERPARAMETERFVPROC                       , glGetSamplerParameterfv                       );  \
    Func ( PFNGLGETSAMPLERPARAMETERIUIVPROC                     , glGetSamplerParameterIuiv                     );  \
    Func ( PFNGLQUERYCOUNTERPROC                                , glQueryCounter                                );  \
    Func ( PFNGLGETQUERYOBJECTI64VPROC                          , glGetQueryObjecti64v                          );  \
    Func ( PFNGLGETQUERYOBJECTUI64VPROC                         , glGetQueryObjectui64v                         );  \
    Func ( PFNGLVERTEXATTRIBDIVISORPROC                         , glVertexAttribDivisor                         );  \
    Func ( PFNGLVERTEXATTRIBP1UIPROC                            , glVertexAttribP1ui                            );  \
    Func ( PFNGLVERTEXATTRIBP1UIVPROC                           , glVertexAttribP1uiv                           );  \
    Func ( PFNGLVERTEXATTRIBP2UIPROC                            , glVertexAttribP2ui                            );  \
    Func ( PFNGLVERTEXATTRIBP2UIVPROC                           , glVertexAttribP2uiv                           );  \
    Func ( PFNGLVERTEXATTRIBP3UIPROC                            , glVertexAttribP3ui                            );  \
    Func ( PFNGLVERTEXATTRIBP3UIVPROC                           , glVertexAttribP3uiv                           );  \
    Func ( PFNGLVERTEXATTRIBP4UIPROC                            , glVertexAttribP4ui                            );  \
    Func ( PFNGLVERTEXATTRIBP4UIVPROC                           , glVertexAttribP4uiv                           );  \
                                                                                                                    \
    /* GL_VERSION_4_0 */                                                                                            \
    Func ( PFNGLMINSAMPLESHADINGPROC                            , glMinSampleShading                            );  \
    Func ( PFNGLBLENDEQUATIONIPROC                              , glBlendEquationi                              );  \
    Func ( PFNGLBLENDEQUATIONSEPARATEIPROC                      , glBlendEquationSeparatei                      );  \
    Func ( PFNGLBLENDFUNCIPROC                                  , glBlendFunci                                  );  \
    Func ( PFNGLBLENDFUNCSEPARATEIPROC                          , glBlendFuncSeparatei                          );  \
    Func ( PFNGLDRAWARRAYSINDIRECTPROC                          , glDrawArraysIndirect                          );  \
    Func ( PFNGLDRAWELEMENTSINDIRECTPROC                        , glDrawElementsIndirect                        );  \
    Func ( PFNGLUNIFORM1DPROC                                   , glUniform1d                                   );  \
    Func ( PFNGLUNIFORM2DPROC                                   , glUniform2d                                   );  \
    Func ( PFNGLUNIFORM3DPROC                                   , glUniform3d                                   );  \
    Func ( PFNGLUNIFORM4DPROC                                   , glUniform4d                                   );  \
    Func ( PFNGLUNIFORM1DVPROC                                  , glUniform1dv                                  );  \
    Func ( PFNGLUNIFORM2DVPROC                                  , glUniform2dv                                  );  \
    Func ( PFNGLUNIFORM3DVPROC                                  , glUniform3dv                                  );  \
    Func ( PFNGLUNIFORM4DVPROC                                  , glUniform4dv                                  );  \
    Func ( PFNGLUNIFORMMATRIX2DVPROC                            , glUniformMatrix2dv                            );  \
    Func ( PFNGLUNIFORMMATRIX3DVPROC                            , glUniformMatrix3dv                            );  \
    Func ( PFNGLUNIFORMMATRIX4DVPROC                            , glUniformMatrix4dv                            );  \
    Func ( PFNGLUNIFORMMATRIX2X3DVPROC                          , glUniformMatrix2x3dv                          );  \
    Func ( PFNGLUNIFORMMATRIX2X4DVPROC                          , glUniformMatrix2x4dv                          );  \
    Func ( PFNGLUNIFORMMATRIX3X2DVPROC                          , glUniformMatrix3x2dv                          );  \
    Func ( PFNGLUNIFORMMATRIX3X4DVPROC                          , glUniformMatrix3x4dv                          );  \
    Func ( PFNGLUNIFORMMATRIX4X2DVPROC                          , glUniformMatrix4x2dv                          );  \
    Func ( PFNGLUNIFORMMATRIX4X3DVPROC                          , glUniformMatrix4x3dv                          );  \
    Func ( PFNGLGETUNIFORMDVPROC                                , glGetUniformdv                                );  \
    Func ( PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC                , glGetSubroutineUniformLocation                );  \
    Func ( PFNGLGETSUBROUTINEINDEXPROC                          , glGetSubroutineIndex                          );  \
    Func ( PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC                , glGetActiveSubroutineUniformiv                );  \
    Func ( PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC              , glGetActiveSubroutineUniformName              );  \
    Func ( PFNGLGETACTIVESUBROUTINENAMEPROC                     , glGetActiveSubroutineName                     );  \
    Func ( PFNGLUNIFORMSUBROUTINESUIVPROC                       , glUniformSubroutinesuiv                       );  \
    Func ( PFNGLGETUNIFORMSUBROUTINEUIVPROC                     , glGetUniformSubroutineuiv                     );  \
    Func ( PFNGLGETPROGRAMSTAGEIVPROC                           , glGetProgramStageiv                           );  \
    Func ( PFNGLPATCHPARAMETERIPROC                             , glPatchParameteri                             );  \
    Func ( PFNGLPATCHPARAMETERFVPROC                            , glPatchParameterfv                            );  \
    Func ( PFNGLBINDTRANSFORMFEEDBACKPROC                       , glBindTransformFeedback                       );  \
    Func ( PFNGLDELETETRANSFORMFEEDBACKSPROC                    , glDeleteTransformFeedbacks                    );  \
    Func ( PFNGLGENTRANSFORMFEEDBACKSPROC                       , glGenTransformFeedbacks                       );  \
    Func ( PFNGLISTRANSFORMFEEDBACKPROC                         , glIsTransformFeedback                         );  \
    Func ( PFNGLPAUSETRANSFORMFEEDBACKPROC                      , glPauseTransformFeedback                      );  \
    Func ( PFNGLRESUMETRANSFORMFEEDBACKPROC                     , glResumeTransformFeedback                     );  \
    Func ( PFNGLDRAWTRANSFORMFEEDBACKPROC                       , glDrawTransformFeedback                       );  \
    Func ( PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC                 , glDrawTransformFeedbackStream                 );  \
    Func ( PFNGLBEGINQUERYINDEXEDPROC                           , glBeginQueryIndexed                           );  \
    Func ( PFNGLENDQUERYINDEXEDPROC                             , glEndQueryIndexed                             );  \
    Func ( PFNGLGETQUERYINDEXEDIVPROC                           , glGetQueryIndexediv                           );  \
                                                                                                                    \
    /* GL_VERSION_4_1 */                                                                                            \
    Func ( PFNGLRELEASESHADERCOMPILERPROC                       , glReleaseShaderCompiler                       );  \
    Func ( PFNGLSHADERBINARYPROC                                , glShaderBinary                                );  \
    Func ( PFNGLGETSHADERPRECISIONFORMATPROC                    , glGetShaderPrecisionFormat                    );  \
    Func ( PFNGLDEPTHRANGEFPROC                                 , glDepthRangef                                 );  \
    Func ( PFNGLCLEARDEPTHFPROC                                 , glClearDepthf                                 );  \
    Func ( PFNGLGETPROGRAMBINARYPROC                            , glGetProgramBinary                            );  \
    Func ( PFNGLPROGRAMBINARYPROC                               , glProgramBinary                               );  \
    Func ( PFNGLPROGRAMPARAMETERIPROC                           , glProgramParameteri                           );  \
    Func ( PFNGLUSEPROGRAMSTAGESPROC                            , glUseProgramStages                            );  \
    Func ( PFNGLACTIVESHADERPROGRAMPROC                         , glActiveShaderProgram                         );  \
    Func ( PFNGLCREATESHADERPROGRAMVPROC                        , glCreateShaderProgramv                        );  \
    Func ( PFNGLBINDPROGRAMPIPELINEPROC                         , glBindProgramPipeline                         );  \
    Func ( PFNGLDELETEPROGRAMPIPELINESPROC                      , glDeleteProgramPipelines                      );  \
    Func ( PFNGLGENPROGRAMPIPELINESPROC                         , glGenProgramPipelines                         );  \
    Func ( PFNGLISPROGRAMPIPELINEPROC                           , glIsProgramPipeline                           );  \
    Func ( PFNGLGETPROGRAMPIPELINEIVPROC                        , glGetProgramPipelineiv                        );  \
    Func ( PFNGLPROGRAMUNIFORM1IPROC                            , glProgramUniform1i                            );  \
    Func ( PFNGLPROGRAMUNIFORM1IVPROC                           , glProgramUniform1iv                           );  \
    Func ( PFNGLPROGRAMUNIFORM1FPROC                            , glProgramUniform1f                            );  \
    Func ( PFNGLPROGRAMUNIFORM1FVPROC                           , glProgramUniform1fv                           );  \
    Func ( PFNGLPROGRAMUNIFORM1DPROC                            , glProgramUniform1d                            );  \
    Func ( PFNGLPROGRAMUNIFORM1DVPROC                           , glProgramUniform1dv                           );  \
    Func ( PFNGLPROGRAMUNIFORM1UIPROC                           , glProgramUniform1ui                           );  \
    Func ( PFNGLPROGRAMUNIFORM1UIVPROC                          , glProgramUniform1uiv                          );  \
    Func ( PFNGLPROGRAMUNIFORM2IPROC                            , glProgramUniform2i                            );  \
    Func ( PFNGLPROGRAMUNIFORM2IVPROC                           , glProgramUniform2iv                           );  \
    Func ( PFNGLPROGRAMUNIFORM2FPROC                            , glProgramUniform2f                            );  \
    Func ( PFNGLPROGRAMUNIFORM2FVPROC                           , glProgramUniform2fv                           );  \
    Func ( PFNGLPROGRAMUNIFORM2DPROC                            , glProgramUniform2d                            );  \
    Func ( PFNGLPROGRAMUNIFORM2DVPROC                           , glProgramUniform2dv                           );  \
    Func ( PFNGLPROGRAMUNIFORM2UIPROC                           , glProgramUniform2ui                           );  \
    Func ( PFNGLPROGRAMUNIFORM2UIVPROC                          , glProgramUniform2uiv                          );  \
    Func ( PFNGLPROGRAMUNIFORM3IPROC                            , glProgramUniform3i                            );  \
    Func ( PFNGLPROGRAMUNIFORM3IVPROC                           , glProgramUniform3iv                           );  \
    Func ( PFNGLPROGRAMUNIFORM3FPROC                            , glProgramUniform3f                            );  \
    Func ( PFNGLPROGRAMUNIFORM3FVPROC                           , glProgramUniform3fv                           );  \
    Func ( PFNGLPROGRAMUNIFORM3DPROC                            , glProgramUniform3d                            );  \
    Func ( PFNGLPROGRAMUNIFORM3DVPROC                           , glProgramUniform3dv                           );  \
    Func ( PFNGLPROGRAMUNIFORM3UIPROC                           , glProgramUniform3ui                           );  \
    Func ( PFNGLPROGRAMUNIFORM3UIVPROC                          , glProgramUniform3uiv                          );  \
    Func ( PFNGLPROGRAMUNIFORM4IPROC                            , glProgramUniform4i                            );  \
    Func ( PFNGLPROGRAMUNIFORM4IVPROC                           , glProgramUniform4iv                           );  \
    Func ( PFNGLPROGRAMUNIFORM4FPROC                            , glProgramUniform4f                            );  \
    Func ( PFNGLPROGRAMUNIFORM4FVPROC                           , glProgramUniform4fv                           );  \
    Func ( PFNGLPROGRAMUNIFORM4DPROC                            , glProgramUniform4d                            );  \
    Func ( PFNGLPROGRAMUNIFORM4DVPROC                           , glProgramUniform4dv                           );  \
    Func ( PFNGLPROGRAMUNIFORM4UIPROC                           , glProgramUniform4ui                           );  \
    Func ( PFNGLPROGRAMUNIFORM4UIVPROC                          , glProgramUniform4uiv                          );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX2FVPROC                     , glProgramUniformMatrix2fv                     );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX3FVPROC                     , glProgramUniformMatrix3fv                     );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX4FVPROC                     , glProgramUniformMatrix4fv                     );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX2DVPROC                     , glProgramUniformMatrix2dv                     );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX3DVPROC                     , glProgramUniformMatrix3dv                     );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX4DVPROC                     , glProgramUniformMatrix4dv                     );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC                   , glProgramUniformMatrix2x3fv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC                   , glProgramUniformMatrix3x2fv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC                   , glProgramUniformMatrix2x4fv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC                   , glProgramUniformMatrix4x2fv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC                   , glProgramUniformMatrix3x4fv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC                   , glProgramUniformMatrix4x3fv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC                   , glProgramUniformMatrix2x3dv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC                   , glProgramUniformMatrix3x2dv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC                   , glProgramUniformMatrix2x4dv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC                   , glProgramUniformMatrix4x2dv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC                   , glProgramUniformMatrix3x4dv                   );  \
    Func ( PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC                   , glProgramUniformMatrix4x3dv                   );  \
    Func ( PFNGLVALIDATEPROGRAMPIPELINEPROC                     , glValidateProgramPipeline                     );  \
    Func ( PFNGLGETPROGRAMPIPELINEINFOLOGPROC                   , glGetProgramPipelineInfoLog                   );  \
    Func ( PFNGLVERTEXATTRIBL1DPROC                             , glVertexAttribL1d                             );  \
    Func ( PFNGLVERTEXATTRIBL2DPROC                             , glVertexAttribL2d                             );  \
    Func ( PFNGLVERTEXATTRIBL3DPROC                             , glVertexAttribL3d                             );  \
    Func ( PFNGLVERTEXATTRIBL4DPROC                             , glVertexAttribL4d                             );  \
    Func ( PFNGLVERTEXATTRIBL1DVPROC                            , glVertexAttribL1dv                            );  \
    Func ( PFNGLVERTEXATTRIBL2DVPROC                            , glVertexAttribL2dv                            );  \
    Func ( PFNGLVERTEXATTRIBL3DVPROC                            , glVertexAttribL3dv                            );  \
    Func ( PFNGLVERTEXATTRIBL4DVPROC                            , glVertexAttribL4dv                            );  \
    Func ( PFNGLVERTEXATTRIBLPOINTERPROC                        , glVertexAttribLPointer                        );  \
    Func ( PFNGLGETVERTEXATTRIBLDVPROC                          , glGetVertexAttribLdv                          );  \
    Func ( PFNGLVIEWPORTARRAYVPROC                              , glViewportArrayv                              );  \
    Func ( PFNGLVIEWPORTINDEXEDFPROC                            , glViewportIndexedf                            );  \
    Func ( PFNGLVIEWPORTINDEXEDFVPROC                           , glViewportIndexedfv                           );  \
    Func ( PFNGLSCISSORARRAYVPROC                               , glScissorArrayv                               );  \
    Func ( PFNGLSCISSORINDEXEDPROC                              , glScissorIndexed                              );  \
    Func ( PFNGLSCISSORINDEXEDVPROC                             , glScissorIndexedv                             );  \
    Func ( PFNGLDEPTHRANGEARRAYVPROC                            , glDepthRangeArrayv                            );  \
    Func ( PFNGLDEPTHRANGEINDEXEDPROC                           , glDepthRangeIndexed                           );  \
    Func ( PFNGLGETFLOATI_VPROC                                 , glGetFloati_v                                 );  \
    Func ( PFNGLGETDOUBLEI_VPROC                                , glGetDoublei_v                                );  \
                                                                                                                    \
    /* GL_VERSION_4_2 */                                                                                            \
    Func ( PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC             , glDrawArraysInstancedBaseInstance             );  \
    Func ( PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC           , glDrawElementsInstancedBaseInstance           );  \
    Func ( PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC , glDrawElementsInstancedBaseVertexBaseInstance );  \
    Func ( PFNGLGETINTERNALFORMATI64VPROC                       , glGetInternalformati64v                       );  \
    Func ( PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC              , glGetActiveAtomicCounterBufferiv              );  \
    Func ( PFNGLBINDIMAGETEXTUREPROC                            , glBindImageTexture                            );  \
    Func ( PFNGLMEMORYBARRIERPROC                               , glMemoryBarrier                               );  \
    Func ( PFNGLTEXSTORAGE1DPROC                                , glTexStorage1D                                );  \
    Func ( PFNGLTEXSTORAGE2DPROC                                , glTexStorage2D                                );  \
    Func ( PFNGLTEXSTORAGE3DPROC                                , glTexStorage3D                                );  \
    Func ( PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC              , glDrawTransformFeedbackInstanced              );  \
    Func ( PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC        , glDrawTransformFeedbackStreamInstanced        );  \
                                                                                                                    \
    /* GL_VERSION_4_3 */                                                                                            \
    Func ( PFNGLCLEARBUFFERDATAPROC                             , glClearBufferData                             );  \
    Func ( PFNGLCLEARBUFFERSUBDATAPROC                          , glClearBufferSubData                          );  \
    Func ( PFNGLDISPATCHCOMPUTEPROC                             , glDispatchCompute                             );  \
    Func ( PFNGLDISPATCHCOMPUTEINDIRECTPROC                     , glDispatchComputeIndirect                     );  \
    Func ( PFNGLCOPYIMAGESUBDATAPROC                            , glCopyImageSubData                            );  \
    Func ( PFNGLFRAMEBUFFERPARAMETERIPROC                       , glFramebufferParameteri                       );  \
    Func ( PFNGLGETFRAMEBUFFERPARAMETERIVPROC                   , glGetFramebufferParameteriv                   );  \
    Func ( PFNGLINVALIDATETEXSUBIMAGEPROC                       , glInvalidateTexSubImage                       );  \
    Func ( PFNGLINVALIDATETEXIMAGEPROC                          , glInvalidateTexImage                          );  \
    Func ( PFNGLINVALIDATEBUFFERSUBDATAPROC                     , glInvalidateBufferSubData                     );  \
    Func ( PFNGLINVALIDATEBUFFERDATAPROC                        , glInvalidateBufferData                        );  \
    Func ( PFNGLINVALIDATEFRAMEBUFFERPROC                       , glInvalidateFramebuffer                       );  \
    Func ( PFNGLINVALIDATESUBFRAMEBUFFERPROC                    , glInvalidateSubFramebuffer                    );  \
    Func ( PFNGLMULTIDRAWARRAYSINDIRECTPROC                     , glMultiDrawArraysIndirect                     );  \
    Func ( PFNGLMULTIDRAWELEMENTSINDIRECTPROC                   , glMultiDrawElementsIndirect                   );  \
    Func ( PFNGLGETPROGRAMINTERFACEIVPROC                       , glGetProgramInterfaceiv                       );  \
    Func ( PFNGLGETPROGRAMRESOURCEINDEXPROC                     , glGetProgramResourceIndex                     );  \
    Func ( PFNGLGETPROGRAMRESOURCENAMEPROC                      , glGetProgramResourceName                      );  \
    Func ( PFNGLGETPROGRAMRESOURCEIVPROC                        , glGetProgramResourceiv                        );  \
    Func ( PFNGLGETPROGRAMRESOURCELOCATIONPROC                  , glGetProgramResourceLocation                  );  \
    Func ( PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC             , glGetProgramResourceLocationIndex             );  \
    Func ( PFNGLSHADERSTORAGEBLOCKBINDINGPROC                   , glShaderStorageBlockBinding                   );  \
    Func ( PFNGLTEXBUFFERRANGEPROC                              , glTexBufferRange                              );  \
    Func ( PFNGLTEXSTORAGE2DMULTISAMPLEPROC                     , glTexStorage2DMultisample                     );  \
    Func ( PFNGLTEXSTORAGE3DMULTISAMPLEPROC                     , glTexStorage3DMultisample                     );  \
    Func ( PFNGLTEXTUREVIEWPROC                                 , glTextureView                                 );  \
    Func ( PFNGLBINDVERTEXBUFFERPROC                            , glBindVertexBuffer                            );  \
    Func ( PFNGLVERTEXATTRIBFORMATPROC                          , glVertexAttribFormat                          );  \
    Func ( PFNGLVERTEXATTRIBIFORMATPROC                         , glVertexAttribIFormat                         );  \
    Func ( PFNGLVERTEXATTRIBLFORMATPROC                         , glVertexAttribLFormat                         );  \
    Func ( PFNGLVERTEXATTRIBBINDINGPROC                         , glVertexAttribBinding                         );  \
    Func ( PFNGLVERTEXBINDINGDIVISORPROC                        , glVertexBindingDivisor                        );  \
    Func ( PFNGLDEBUGMESSAGECONTROLPROC                         , glDebugMessageControl                         );  \
    Func ( PFNGLDEBUGMESSAGEINSERTPROC                          , glDebugMessageInsert                          );  \
    Func ( PFNGLDEBUGMESSAGECALLBACKPROC                        , glDebugMessageCallback                        );  \
    Func ( PFNGLGETDEBUGMESSAGELOGPROC                          , glGetDebugMessageLog                          );  \
    Func ( PFNGLPUSHDEBUGGROUPPROC                              , glPushDebugGroup                              );  \
    Func ( PFNGLPOPDEBUGGROUPPROC                               , glPopDebugGroup                               );  \
    Func ( PFNGLOBJECTLABELPROC                                 , glObjectLabel                                 );  \
    Func ( PFNGLGETOBJECTLABELPROC                              , glGetObjectLabel                              );  \
    Func ( PFNGLOBJECTPTRLABELPROC                              , glObjectPtrLabel                              );  \
    Func ( PFNGLGETOBJECTPTRLABELPROC                           , glGetObjectPtrLabel                           );  \
                                                                                                                    \
    /* GL_VERSION_4_4 */                                                                                            \
    Func ( PFNGLBUFFERSTORAGEPROC                               , glBufferStorage                               );  \
    Func ( PFNGLCLEARTEXIMAGEPROC                               , glClearTexImage                               );  \
    Func ( PFNGLCLEARTEXSUBIMAGEPROC                            , glClearTexSubImage                            );  \
    Func ( PFNGLBINDBUFFERSBASEPROC                             , glBindBuffersBase                             );  \
    Func ( PFNGLBINDBUFFERSRANGEPROC                            , glBindBuffersRange                            );  \
    Func ( PFNGLBINDTEXTURESPROC                                , glBindTextures                                );  \
    Func ( PFNGLBINDSAMPLERSPROC                                , glBindSamplers                                );  \
    Func ( PFNGLBINDIMAGETEXTURESPROC                           , glBindImageTextures                           );  \
    Func ( PFNGLBINDVERTEXBUFFERSPROC                           , glBindVertexBuffers                           )

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

        static int CreatePixelFormat( HDC hDC, const int * attribList );
    };
}
