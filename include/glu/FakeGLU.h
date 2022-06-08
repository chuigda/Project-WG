#ifndef PRWG_FAKEGLU_H
#define PRWG_FAKEGLU_H

#include <QtGui/qopengl.h>

class QOpenGLFunctions_2_0;

namespace glu {

void Perspective(QOpenGLFunctions_2_0 *f,
                 GLdouble fovY,
                 GLdouble aspect,
                 GLdouble zNear,
                 GLdouble zFar) noexcept;

} // namespace glu

#endif // PRWG_FAKEGLU_H
