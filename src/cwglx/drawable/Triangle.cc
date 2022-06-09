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
    const std::array<std::pair<GLfloat, GLfloat>, 3>& textureCoords =
        g_TriangleTextureCoords[m_ClipMode];

    f->glPushAttrib(GL_TEXTURE_BIT);
    f->glNormal3f(0.0f, 0.0f, 1.0f);
    m_Texture->ApplyTexture(f, textureCoords.data(), 3);
  }

  if (m_Colors.has_value()) {
    ApplyColorArray(f, m_Colors.value().data(), 3);
  }

  f->glVertexPointer(3, GL_DOUBLE, 0, m_Vertices.data());
  f->glDrawArrays(GL_TRIANGLES, 0, 3);

  if (m_Texture) {
    f->glPopAttrib();
  }
  f->glPopAttrib();
}

} // namespace cw
