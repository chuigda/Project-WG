#include "include/wgc0310/ScreenGlass.h"
#include "include/wgc0310/ScreenCurveHelper.h"

namespace wgc0310 {

std::unique_ptr<cw::TriangleGen> ScreenGlass() {
  std::vector<std::vector<cw::Vertex>> vertices =
    ComputeScreenVertices(25.0, 18.75, 1.25, 160, 120);
  std::vector<cw::Triangle> storedTriangles;
  storedTriangles.reserve(160 * 120 * 2);

  for (std::size_t x = 0; x < 160; x++) {
    for (std::size_t y = 0; y < 120; y++) {
      cw::Vertex pointA = vertices[y][x];
      cw::Vertex pointB = vertices[y + 1][x];
      cw::Vertex pointC = vertices[y + 1][x + 1];
      cw::Vertex pointD = vertices[y][x + 1];

      storedTriangles.push_back(cw::Triangle { pointA, pointB, pointC });
      storedTriangles.push_back(cw::Triangle { pointA, pointC, pointD });
    }
  }

  return std::make_unique<cw::StoredTriangles>(std::move(storedTriangles));
}

} // namespace wgc0310
