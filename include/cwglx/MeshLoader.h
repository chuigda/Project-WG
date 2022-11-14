#ifndef PROJECT_WG_MESHLOADER_H
#define PROJECT_WG_MESHLOADER_H

#include <memory>
#include "cwglx/drawable/PlainTriangles.h"

namespace cw {

std::unique_ptr<PlainTriangles> LoadMesh(const char *meshFile);

} // namespace cw

#endif // PROJECT_WG_MESHLOADER_H
