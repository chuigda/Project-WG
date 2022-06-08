#include "cwglx/Color.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

RGBAColor::RGBAColor(std::array<std::uint8_t, 4> repr) noexcept
  : m_Repr(repr)
{}

RGBAColor::RGBAColor(std::uint8_t red,
                               std::uint8_t green,
                               std::uint8_t blue,
                               std::uint8_t alpha) noexcept
  : m_Repr({red, green, blue, alpha})
{}

RGBAColor::RGBAColor(float red,
                     float green,
                     float blue,
                     float alpha) noexcept
  : m_Repr({ static_cast<std::uint8_t>(red * 255.0f),
             static_cast<std::uint8_t>(green * 255.0f),
             static_cast<std::uint8_t>(blue * 255.0f),
             static_cast<std::uint8_t>(alpha * 255.0f) })
{
}

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

void RGBAColorFloat::Apply(QOpenGLFunctions_2_0 *f) noexcept {
  f->glColor4fv(m_Repr.data());
}

} // namespace cw
