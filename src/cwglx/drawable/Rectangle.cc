#include "cwglx/drawable/Rectangle.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

Rectangle::Rectangle(const std::array<Vertex, 4> &vertices,
                     const std::optional<std::array<RGBAColor, 4>> &colors,
                     const Material *material,
                     const Texture2D *texture) noexcept
  : m_Vertices(vertices),
    m_Colors(std::nullopt),
    m_Material(material),
    m_Texture(texture)
{
  if (colors.has_value()) {
    const std::array<RGBAColor, 4> &colorsInt = colors.value();
    m_Colors = std::experimental::make_array(
        RGBAColorF(colorsInt[0]),
        RGBAColorF(colorsInt[1]),
        RGBAColorF(colorsInt[2]),
        RGBAColorF(colorsInt[3])
    );
  }
}

Rectangle::~Rectangle() = default;

void Rectangle::Draw(QOpenGLFunctions_2_0 *f) const noexcept {
  f->glPushAttrib(GL_CURRENT_BIT);

  if (m_Material) {
    f->glNormal3f(0.0f, 0.0f, 1.0f);
    m_Material->Apply(f);
  }

  if (m_Texture) {
    m_Texture->BeginTexture(f);
  }

  f->glBegin(GL_QUADS);
  for (std::size_t i = 0; i < 4; ++i) {
    if (m_Colors.has_value()) {
      m_Colors.value()[i].Apply(f);
    }

    if (m_Texture) {
      const auto &[tx, ty] = g_QuadTextureCoords[i];
      m_Texture->ApplyTexture(f, tx, ty);
    }

    f->glVertex3dv(m_Vertices[i].GetRepr().data());
  }
  f->glEnd();

  f->glPopAttrib();
}

} // namespace cw
