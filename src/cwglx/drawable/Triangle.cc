#include "cwglx/drawable/Triangle.h"

#include <experimental/array>
#include <QOpenGLFunctions_2_0>

namespace cw {

Triangle::Triangle(const std::array<Vertex, 3> &vertexes,
                   const std::optional<std::array<RGBAColor, 3>> &colors,
                   const Material *material,
                   const Texture2D *texture,
                   Triangle::TextureClipMode clipMode) noexcept
  : m_Vertexes(vertexes),
    m_Colors(std::nullopt),
    m_Material(material),
    m_Texture(texture),
    m_ClipMode(clipMode)
{
  if (colors.has_value()) {
    const std::array<RGBAColor, 3> &colorsInt = colors.value();
    for (const auto& color : colorsInt) {
      m_DeferRender |= color.IsTransparent();
    }

    m_Colors = std::experimental::make_array(
        RGBAColorFloat(colorsInt[0]),
        RGBAColorFloat(colorsInt[1]),
        RGBAColorFloat(colorsInt[2])
    );
  }

  if (material) {
    m_DeferRender |= material->IsTransparent();
  }
}

constexpr std::array<std::array<std::pair<GLfloat, GLfloat>, 3>, 3>
TriangleTextureCoords = std::experimental::make_array(
    std::experimental::make_array(
       std::make_pair(0.0f, 1.0f),
       std::make_pair(0.0f, 0.0f),
       std::make_pair(1.0f, 0.0f)
    ),
    std::experimental::make_array(
       std::make_pair(1.0f, 1.0f),
       std::make_pair(0.0f, 0.0f),
       std::make_pair(1.0f, 0.0f)
    ),
    std::experimental::make_array(
       std::make_pair(0.5f, 1.0f),
       std::make_pair(0.0f, 0.0f),
       std::make_pair(1.0f, 0.0f)
    )
);

void Triangle::Draw(QOpenGLFunctions_2_0 *f) const noexcept {
  f->glPushAttrib(GL_CURRENT_BIT);

  if (m_Material) {
    m_Material->Apply(f);
  }

  if (m_Texture) {
    m_Texture->BeginTexture(f);
  }

  const std::array<std::pair<GLfloat, GLfloat>, 3>& textureCoords =
      TriangleTextureCoords[m_ClipMode];

  f->glBegin(GL_TRIANGLES);
  for (std::size_t i = 0; i < 3; ++i) {
    if (m_Colors.has_value()) {
      m_Colors.value()[i].Apply(f);
    }

    if (m_Texture) {
      const auto& [tx, ty] = textureCoords[i];
      m_Texture->ApplyTexture(f, tx, ty);
    }

    f->glVertex3dv(m_Vertexes[i].GetRepr().data());
  }
  f->glEnd();

  f->glPopAttrib();
}

} // namespace cw
