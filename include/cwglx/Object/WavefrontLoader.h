#ifndef PROJECT_GL2_WAVEFRONT_LOADER_H
#define PROJECT_GL2_WAVEFRONT_LOADER_H

#include <vector>
#include "Vertex.h"

namespace cw {

std::vector<SimpleVertex> LoadMesh(QString const& fileName, double ratio = 1.0);
std::vector<PlainVertex> LoadLineMesh(QString const& fileName, double ratio = 1.0);

} // namespace cw

#endif // PROJECT_GL2_WAVEFRONT_LOADER_H
