#ifndef PROJECT_WG_TRIANGLE_H
#define PROJECT_WG_TRIANGLE_H

#include <optional>

#include "cwglx/Vertex.h"
#include "cwglx/Material.h"
#include "cwglx/Texture.h"
#include "cwglx/Drawable.h"

namespace cw {

class Triangle final : Drawable {
public:
  enum TextureClipMode {
    ClipLeft = 0,
    ClipRight = 1,
    ClipMiddle = 2
  };

  Triangle(const std::array<Vertex, 3>& vertexes,
           const std::optional<std::array<RGBAColor, 3>>& colors = std::nullopt,
           const Material *material = nullptr,
           const Texture2D *texture = nullptr,
           TextureClipMode clipMode = ClipLeft) noexcept;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

private:
  std::array<Vertex, 3> m_Vertexes;
  std::optional<std::array<RGBAColorFloat, 3>> m_Colors;
  const Material *m_Material;
  const Texture2D *m_Texture;
  TextureClipMode m_ClipMode;
  bool m_DeferRender;
};

} // namespace cw

#endif // PROJECT_WG_TRIANGLE_H
