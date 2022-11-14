#include "include/wgc0310/ScreenCurveHelper.h"

#include <cmath>
#include <QDebug>
#include "util/Constants.h"

namespace wgc0310 {

/// computes the X-Z mapping of vertical cylinder
static std::vector<std::pair<double, double>>
ComputeVCylinder(double surfaceWidth,
                 double bulb,
                 std::size_t segments) {
  double radius = (bulb / 2) + (surfaceWidth * surfaceWidth / (8 * bulb));
  double base = radius - bulb;
  double startAngleRad = std::asin(surfaceWidth / (2 * radius));
  double startAngle = -cw::RadToDeg(startAngleRad);
  double endAngle = -startAngle;

  double angleStep = (endAngle - startAngle) / static_cast<double>(segments);

  std::vector<std::pair<double, double>> result;
  result.reserve(segments + 1);
  for (std::size_t i = 0; i <= segments; i++) {
    double angle = startAngle + static_cast<double>(i) * angleStep;
    double x = std::sin(cw::DegToRad(angle)) * radius;
    double z = std::cos(cw::DegToRad(angle)) * radius - base;

    result.emplace_back(x, z);
  }

  return result;
}

/// computes the Y-Z mapping of horizontal cylinder
static std::vector<std::pair<double, double>>
ComputeHCylinder(double surfaceHeight,
                 double bulb,
                 std::size_t segments) {
  // actually there's no implementation difference between vSegments and
  // hSegments, just forward to the vSegments version

  return ComputeVCylinder(surfaceHeight, bulb, segments);
}

std::vector<std::vector<cw::Vertex>>
ComputeScreenVertices(double screenWidth,
                      double screenHeight,
                      double bulb,
                      std::size_t hSegments,
                      std::size_t vSegments) {
  Q_ASSERT(hSegments % 2 == 0
           && vSegments % 2 == 0
           && "non-even segments could be inaccurate and is thus prohibited");

  std::vector<std::pair<double, double>> vCylinder =
      ComputeVCylinder(screenWidth, bulb / 2.0, hSegments);
  std::vector<std::pair<double, double>> hCylinder =
      ComputeHCylinder(screenHeight, bulb / 2.0, vSegments);

  // compute the vertices
  std::vector<std::vector<cw::Vertex>> result;
  result.reserve(vSegments + 1);

  for (std::size_t i = 0; i <= vSegments; i++) {
    const auto& [y, z1] = hCylinder[vSegments - i];

    std::vector<cw::Vertex> line;
    line.reserve(hSegments + 1);

    for (std::size_t j = 0; j <= hSegments; j++) {
      const auto& [x, z2] = vCylinder[j];

      line.emplace_back(x, y, z1 + z2);
    }

    result.push_back(std::move(line));
  }

  return result;
}

} // namespace wgc0310
