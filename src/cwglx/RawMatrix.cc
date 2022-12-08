#include "cwglx/RawMatrix.h"

#include "cwglx/GLImpl.h"

namespace cw {

RawMatrix RawMatrix::GetFromContext(GLFunctions *f) {
  RawMatrix r;
  f->glGetFloatv(GL_MODELVIEW_MATRIX, r.rawMatrix.data());
  return r;
}

void RawMatrix::SetToContext(GLFunctions *f) const noexcept {
  f->glLoadMatrixf(rawMatrix.data());
}

} // namespace cw
