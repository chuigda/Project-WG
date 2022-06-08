#include "glu/FakeGLU.h"

#include <QOpenGLFunctions_1_5>
#include "util/Constants.h"

namespace glu {

void perspective(QOpenGLFunctions_1_5 *f,
                 GLdouble fovY,
                 GLdouble aspect,
                 GLdouble zNear,
                 GLdouble zFar) noexcept
{
    GLdouble fH = std::tan((fovY / 360) * cw::constants::PI) * zNear;
    GLdouble fW = fH * aspect;
    f->glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

} // namespace glu
