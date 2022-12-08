#ifndef PROJECT_WG_RAW_MATRIX_H
#define PROJECT_WG_RAW_MATRIX_H

#include <array>
#include <QtGui/qopengl.h>
#include "cwglx/GL.h"

namespace cw {

// not a matrix for very general purpose, just used to store OpenGL status
struct RawMatrix {
  std::array<GLfloat, 16> rawMatrix {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  static RawMatrix GetFromContext(GLFunctions *f);
  void SetToContext(GLFunctions *f) const noexcept;
};

} // namespace cw

#endif // PROJECT_WG_RAW_MATRIX_H
