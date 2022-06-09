#ifndef PROJECT_WG_RECTANGLE_H
#define PROJECT_WG_RECTANGLE_H

#include <array>
#include <optional>
#include <experimental/array>

#include "cwglx/Vertex.h"
#include "cwglx/Material.h"
#include "cwglx/Texture.h"
#include "cwglx/Drawable.h"

namespace cw {

constexpr std::array<std::pair<GLfloat, GLfloat>, 4>
g_QuadTextureCoords = std::experimental::make_array(
    std::make_pair(1.0f, 1.0f),
    std::make_pair(0.0f, 1.0f),
    std::make_pair(0.0f, 0.0f),
    std::make_pair(1.0f, 0.0f)
);

class Rectangle final : Drawable {
public:
  Rectangle(const std::array<Vertex, 4>& vertices,
            const std::optional<std::array<RGBAColor, 4>>& colors = std::nullopt,
            const Material *material = nullptr,
            const Texture2D *texture = nullptr) noexcept;

  ~Rectangle() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

private:
  std::array<Vertex, 4> m_Vertices;
  std::optional<std::array<RGBAColorF, 4>> m_Colors;
  const Material *m_Material;
  const Texture2D *m_Texture;
};

} // namespace cw

#endif //PROJECT_WG_RECTANGLE_H
