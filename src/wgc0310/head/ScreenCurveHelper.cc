#include "wgc0310/head/ScreenCurveHelper.h"

namespace wgc0310 {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-integer-division"
std::vector<std::vector<cw::Vertex>>
ComputeScreenVertices(double screenWidth,
                      double screenHeight,
                      double bulb,
                      int horizontalSegments,
                      int verticalSegments) {
  Q_ASSERT(horizontalSegments % 2 == 0
           && verticalSegments % 2 == 0
           && "non-even segments could be inaccurate and is thus prohibited");

  std::vector<std::vector<cw::Vertex>> vertices;
  vertices.reserve(verticalSegments);

  double segmentWidth = screenWidth / static_cast<double>(horizontalSegments);
  double segmentHeight = screenHeight / static_cast<double>(verticalSegments);

  double radius = (bulb / 2) + (screenHeight * screenHeight / (8 * bulb));

  for (int ySegment = 0; ySegment < verticalSegments; ySegment++) {
    double segmentY = segmentHeight *
        static_cast<GLdouble>((verticalSegments / 2) - ySegment);
    double z = std::sqrt(radius * radius - segmentY * segmentY);
    double z1 = z - radius + bulb;

    std::vector<cw::Vertex> lineVertices;
    lineVertices.reserve(horizontalSegments);

    if (std::abs(z1) <= 0.0000001) {
      for (int xSegment = 0; xSegment < horizontalSegments; xSegment++) {
        double segmentX = segmentWidth *
                          static_cast<GLdouble>(xSegment - (horizontalSegments / 2));
        lineVertices.emplace_back(segmentX, segmentY, 0.0);
      }
    } else {
      double radius2 = (z1 / 2) + (screenWidth * screenHeight / (8 * z1));
      for (int xSegment = 0; xSegment < horizontalSegments; xSegment++) {
        double segmentX = segmentWidth *
            static_cast<GLdouble>(xSegment - (horizontalSegments / 2));
        double z2 = std::sqrt(radius2 * radius2 - segmentX * segmentX);
        double z3 = std::abs(z2 + z1 - radius2);

        lineVertices.emplace_back(segmentX, segmentY, z3);
      }
    }
    vertices.push_back(std::move(lineVertices));
  }

  return vertices;
}
#pragma clang diagnostic pop

} // namespace wgc0310
