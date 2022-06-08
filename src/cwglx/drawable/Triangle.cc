#include "cwglx/drawable/Triangle.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

Triangle::Triangle(const std::array<Vertex, 3> &vertices,
                   const std::optional<std::array<RGBAColor, 3>> &colors,
                   const Material *material,
                   const Texture2D *texture,
                   Triangle::TextureClipMode clipMode) noexcept
  : m_Vertices(vertices),
    m_Colors(std::nullopt),
    m_Material(material),
    m_Texture(texture),
    m_ClipMode(clipMode)
{
  if (colors.has_value()) {
    const std::array<RGBAColor, 3> &colorsInt = colors.value();
    m_Colors = std::experimental::make_array(
        RGBAColorFloat(colorsInt[0]),
        RGBAColorFloat(colorsInt[1]),
        RGBAColorFloat(colorsInt[2])
    );
  }
}

Triangle::~Triangle() = default;

void Triangle::Draw(QOpenGLFunctions_2_0 *f) const noexcept {
  f->glPushAttrib(GL_CURRENT_BIT);

  if (m_Material) {
    m_Material->Apply(f);
  }

  if (m_Texture) {
    m_Texture->BeginTexture(f);
    f->glNormal3f(0.0f, 0.0f, 1.0f);
  }

  const std::array<std::pair<GLfloat, GLfloat>, 3>& textureCoords =
      g_TriangleTextureCoords[m_ClipMode];

  f->glBegin(GL_TRIANGLES);
  for (std::size_t i = 0; i < 3; ++i) {
    if (m_Colors.has_value()) {
      m_Colors.value()[i].Apply(f);
    }

    if (m_Texture) {
      const auto& [tx, ty] = textureCoords[i];
      m_Texture->ApplyTexture(f, tx, ty);
    }

    f->glVertex3dv(m_Vertices[i].GetRepr().data());
  }
  f->glEnd();

  f->glPopAttrib();
}

} // namespace cw
