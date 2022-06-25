#include "glu/FakeGLU.h"

#include "cwglx/GLImpl.h"
#include "util/Constants.h"

namespace glu {

void Perspective(GLFunctions *f,
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
