#include "cwglx/Setup.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

void SetupPreferredSettings(QOpenGLFunctions_2_0 *f) noexcept {
  f->initializeOpenGLFunctions();

  f->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  f->glFrontFace(GL_CCW);
  f->glDepthFunc(GL_LESS);
  f->glBlendFuncSeparate(GL_ONE,
                         GL_ONE_MINUS_SRC_ALPHA,
                         GL_ONE,
                         GL_ONE_MINUS_SRC_ALPHA);
  f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  f->glShadeModel(GL_SMOOTH);
  f->glEnable(GL_MULTISAMPLE);
  f->glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  f->glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  f->glEnable(GL_COLOR_MATERIAL);
  f->glEnable(GL_TEXTURE_2D);
}

} // namespace cw
