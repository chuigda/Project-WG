#ifndef PROJECT_WG_WGC0310_SCREEN_CURVE_HELPER_H
#define PROJECT_WG_WGC0310_SCREEN_CURVE_HELPER_H

#include <vector>
#include "cwglx/Vertex.h"

namespace wgc0310 {

std::vector<std::vector<cw::Vertex>>
ComputeScreenVertices(double screenWidth,
                      double screenHeight,
                      double bulb,
                      std::size_t horizontalSegments,
                      std::size_t verticalSegments);

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SCREEN_CURVE_HELPER_H
