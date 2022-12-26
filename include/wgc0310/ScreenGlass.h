#ifndef PROJECT_WG_WGC0310_SCREENGLASS_H
#define PROJECT_WG_WGC0310_SCREENGLASS_H

#include <memory>
#include "cwglx/Vertex.h"
#include "cwglx/drawable/PlainMesh.h"

namespace wgc0310 {

std::unique_ptr<cw::PlainMesh> ScreenGlass();

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SCREENGLASS_H
