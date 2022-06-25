#ifndef PROJECT_WG_FAKEGLU_H
#define PROJECT_WG_FAKEGLU_H

#include <QtGui/qopengl.h>

#include "cwglx/GL.h"

namespace glu {

void Perspective(GLFunctions *f,
                 GLdouble fovY,
                 GLdouble aspect,
                 GLdouble zNear,
                 GLdouble zFar) noexcept;

} // namespace glu

#endif // PROJECT_WG_FAKEGLU_H
