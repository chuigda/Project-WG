#include "cwglx/Color.h"

#include "cwglx/GLImpl.h"

namespace cw {

void RGBAColor::Apply(GLFunctions *f) noexcept {
  f->glColor4ubv(m_Repr.data());
}

RGBAColorF::RGBAColorF(const std::array<float, 4> &repr) noexcept
  : m_Repr(repr)
{}

RGBAColorF::RGBAColorF(GLfloat red,
                       GLfloat green,
                       GLfloat blue,
                       GLfloat alpha) noexcept
  : m_Repr({red, green, blue, alpha})
{}

RGBAColorF::RGBAColorF(RGBAColor color) noexcept
  : m_Repr({ static_cast<GLfloat>(color.GetRed()) / 255.0f,
             static_cast<GLfloat>(color.GetGreen()) / 255.0f,
             static_cast<GLfloat>(color.GetBlue()) / 255.0f,
             static_cast<GLfloat>(color.GetAlpha()) / 255.0f })
{}

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
