#ifndef PROJECT_WG_FAN_H
#define PROJECT_WG_FAN_H

#include <functional>
#include <optional>
#include <vector>

#include "cwglx/Vertex.h"
#include "cwglx/Material.h"
#include "cwglx/Texture.h"
#include "cwglx/Drawable.h"

namespace cw {

using FanColorFunc = std::function<RGBAColorF(GLfloat)>;

class Fan final : Drawable {
public:
  Fan(GLdouble radius, GLdouble startAngle, GLdouble endAngle,
      std::size_t numVertices,
      const std::optional<FanColorFunc> &colorFunc = std::nullopt,
      const Material *material = nullptr,
      const Texture2D *texture = nullptr);

  ~Fan() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

private:
  std::vector<Vertex> m_PrecomputedVertices;
  std::vector<RGBAColorF> m_PrecomputedColors;
  const Material *m_Material;
  const Texture2D *m_Texture;
  std::vector<Vertex2D> m_PrecomputedTextureCoords;
};

} // namespace cw

#endif // PROJECT_WG_FAN_H
