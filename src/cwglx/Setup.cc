#include "cwglx/Setup.h"

#include "cwglx/GLImpl.h"

namespace cw {

void SetupPreferredSettings(GLFunctions *f) noexcept {
  f->initializeOpenGLFunctions();

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

  f->glLineWidth(1.5f);

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
