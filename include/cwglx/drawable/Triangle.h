#ifndef PROJECT_WG_TRIANGLE_H
#define PROJECT_WG_TRIANGLE_H

#include <optional>
#include <array>
#include <experimental/array>

#include "cwglx/Vertex.h"
#include "cwglx/Material.h"
#include "cwglx/Texture.h"
#include "cwglx/Drawable.h"

namespace cw {

constexpr std::array<std::array<std::pair<GLfloat, GLfloat>, 3>, 3>
g_TriangleTextureCoords = std::experimental::make_array(
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

class Triangle final : Drawable {
public:
  enum TextureClipMode {
    ClipLeft = 0,
    ClipRight = 1,
    ClipMiddle = 2
  };

  explicit
  Triangle(const std::array<Vertex, 3>& vertices,
           const std::optional<std::array<RGBAColor, 3>>& colors = std::nullopt,
           const Material *material = nullptr,
           const Texture2D *texture = nullptr,
           TextureClipMode clipMode = ClipLeft) noexcept;

  ~Triangle() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

private:
  std::array<VertexF, 3> m_Vertices;
  std::optional<std::array<RGBAColorF, 3>> m_Colors;
  const Material *m_Material;
  const Texture2D *m_Texture;
  TextureClipMode m_ClipMode;
};

} // namespace cw

#endif // PROJECT_WG_TRIANGLE_H
