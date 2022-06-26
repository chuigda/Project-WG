#include "cwglx/Setup.h"

#include <cstdio>
#include <cstdlib>
#include "cwglx/GLImpl.h"

namespace cw {

void GLAPIENTRY OpenGLErrorCallback(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar *message,
                                    const void *userParam) {
  Q_UNUSED(source)
  Q_UNUSED(id)
  Q_UNUSED(length)
  Q_UNUSED(userParam)

  std::fprintf(stderr,
               "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
               (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
               type,
               severity,
               message);
  std::abort();
}

void SetupPreferredSettings(GLFunctions *f) noexcept {
  f->initializeOpenGLFunctions();

  f->glDebugMessageCallback(OpenGLErrorCallback, nullptr);

  f->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  f->glFrontFace(GL_CCW);
  f->glDepthFunc(GL_LESS);
  f->glEnable(GL_CULL_FACE);
  f->glEnable(GL_DEPTH_TEST);
  f->glEnable(GL_BLEND);
  f->glBlendFuncSeparate(GL_ONE,
                         GL_ONE_MINUS_SRC_ALPHA,
                         GL_ONE,
                         GL_ONE_MINUS_SRC_ALPHA);
  f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  f->glShadeModel(GL_SMOOTH);

  f->glEnable(GL_LIGHTING);
  f->glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  f->glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  f->glEnable(GL_NORMALIZE);
  f->glEnable(GL_COLOR_MATERIAL);

  f->glEnable(GL_MULTISAMPLE);
  f->glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  f->glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  f->glEnable(GL_TEXTURE_2D);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
}

} // namespace cw
