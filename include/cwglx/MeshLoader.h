#ifndef PROJECT_WG_MESHLOADER_H
#define PROJECT_WG_MESHLOADER_H

#include <memory>
#include "cwglx/drawable/PlainMesh.h"
#include "cwglx/drawable/PlainLineMesh.h"

namespace cw {

std::unique_ptr<PlainMesh> LoadMesh(const char *meshFile, double ratio = 1.0);
std::unique_ptr<PlainLineMesh> LoadLineMesh(const char *meshFile, double ratio = 1.0);

} // namespace cw

#endif // PROJECT_WG_MESHLOADER_H
