#include "ui_next/GLWindow.h"

void GLWindow::RunWithGLContext(std::function<void(void)> const& f) {
  makeCurrent();
  f();
  doneCurrent();
}
