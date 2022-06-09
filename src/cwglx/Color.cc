#include "cwglx/Color.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

void RGBAColor::Apply(QOpenGLFunctions_2_0 *f) noexcept {
  f->glColor4ubv(m_Repr.data());
}

RGBAColorFloat::RGBAColorFloat(std::array<float, 4> repr) noexcept
  : m_Repr(repr)
{}

RGBAColorFloat::RGBAColorFloat(GLfloat red,
                               GLfloat green,
                               GLfloat blue,
                               GLfloat alpha) noexcept
  : m_Repr({red, green, blue, alpha})
{}

RGBAColorFloat::RGBAColorFloat(RGBAColor color) noexcept
  : m_Repr({ static_cast<GLfloat>(color.GetRed()) / 255.0f,
             static_cast<GLfloat>(color.GetGreen()) / 255.0f,
             static_cast<GLfloat>(color.GetBlue()) / 255.0f,
             static_cast<GLfloat>(color.GetAlpha()) / 255.0f })
{}

void RGBAColorFloat::Apply(QOpenGLFunctions_2_0 *f) const noexcept {
  f->glColor4fv(m_Repr.data());
}

void ApplyColorArray(QOpenGLFunctions_2_0 *f,
                     const RGBAColorFloat *colors,
                     std::size_t numColor) noexcept
{
  Q_UNUSED(numColor)

  f->glEnableClientState(GL_COLOR_ARRAY);
  f->glColorPointer(4, GL_FLOAT, 0, colors);
}
} // namespace cw
