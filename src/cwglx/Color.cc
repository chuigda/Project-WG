#include "cwglx/Color.h"

#include "cwglx/GLImpl.h"

namespace cw {

void RGBAColor::Apply(GLFunctions *f) noexcept {
  f->glColor4ubv(m_Repr.data());
}

void RGBAColorF::Apply(GLFunctions *f) const noexcept {
  f->glColor4fv(m_Repr.data());
}

void ApplyColorArray(GLFunctions *f,
                     const RGBAColorF *colors,
                     std::size_t numColor) noexcept
{
  Q_UNUSED(numColor)

  f->glEnableClientState(GL_COLOR_ARRAY);
  f->glColorPointer(4, GL_FLOAT, 0, colors);
}
} // namespace cw
