#ifndef PROJECT_GL2_WAVEFRONT_LOADER_H
#define PROJECT_GL2_WAVEFRONT_LOADER_H

#include <vector>
#include "Vertex.h"

namespace cw {

class GLObjectContext;

void LoadMaterialLibrary(GLObjectContext *ctx,
                         GLFunctions *f,
                         QString const& fileName);

} // namespace cw

#endif // PROJECT_GL2_WAVEFRONT_LOADER_H
