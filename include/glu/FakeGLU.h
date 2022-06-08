#ifndef PRWG_FAKEGLU_H
#define PRWG_FAKEGLU_H

#include <QtGui/qopengl.h>

class QOpenGLFunctions_1_5;

namespace glu {

void perspective(QOpenGLFunctions_1_5 *f,
                 GLdouble fovY,
                 GLdouble aspect,
                 GLdouble zNear,
                 GLdouble zFar) noexcept;

} // namespace glu

#endif // PRWG_FAKEGLU_H
