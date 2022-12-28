#include "cwglx/Setup.h"
#include "include/cwglx/GL/GLImpl.h"

void cw::SetupPreferred(GLFunctions *f) {
  f->initializeOpenGLFunctions();

  f->glFrontFace(GL_CCW);
  f->glDepthFunc(GL_LESS);
  f->glEnable(GL_CULL_FACE);
  f->glEnable(GL_DEPTH_TEST);

  f->glEnable(GL_MULTISAMPLE);
  f->glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  f->glEnable(GL_BLEND);
  f->glBlendFuncSeparate(GL_ONE,
                         GL_ONE_MINUS_SRC_ALPHA,
                         GL_ONE,
                         GL_ONE_MINUS_SRC_ALPHA);
  f->glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
}
