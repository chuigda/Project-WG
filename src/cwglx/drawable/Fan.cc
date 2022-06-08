#include "cwglx/drawable/Fan.h"

#include <QOpenGLFunctions_2_0>

namespace cw {
Fan::Fan(GLdouble radius,
         GLdouble startAngle,
         GLdouble endAngle,
         std::size_t numVertices,
         const std::optional<FanColorFunc> &colorFunc,
         const Material *material,
         const Texture2D *texture)
{
  // TODO
}

Fan::~Fan() = default;

void Fan::Draw(QOpenGLFunctions_2_0 *f) const noexcept {
  // TODO
}

} // namespace cw
