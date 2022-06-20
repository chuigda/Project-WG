#include "cwglx/Vertex.h"

namespace cw {

Vertex RotateVertex(const Vertex &vertex,
                    const Vertex &centerPoint,
                    Axis axis,
                    GLdouble degree)
{
  Vector diff = vertex - centerPoint;
  const auto [x, y, z] = diff.GetRepr();
  double cos = std::cos(degree);
  double sin = std::sin(degree);
  switch (axis) {
    case Axis::XAxis:
      return {
        x,
        y * cos - z * sin,
        y * sin + z * cos
      };
    case Axis::YAxis:
      return {
        x * cos + z * sin,
        y,
        -x * sin + z * cos
      };
    case Axis::ZAxis:
      return {
        x * cos - y * sin,
        x * sin + y * cos,
        z
      };
  }

  Q_UNREACHABLE();
}

} // namespace cw

