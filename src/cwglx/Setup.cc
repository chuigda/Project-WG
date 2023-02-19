#include "cwglx/Setup.h"
#include "include/cwglx/GL/GLImpl.h"

void cw::SetupPreferred(GLFunctions *f) {
  f->initializeOpenGLFunctions();

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
}
