#ifndef PROJECT_GL2_WAVEFRONT_LOADER_H
#define PROJECT_GL2_WAVEFRONT_LOADER_H

#include <vector>
#include <memory>

#include "cwglx/Object/Object.h"

namespace cw {

class GLObjectContext;

void LoadMaterialLibrary(GLObjectContext *ctx,
                         GLFunctions *f,
                         QString const& basePath,
                         QString const& fileName,
                         bool linearSampling,
                         bool anisotropyFilter);

GLObject LoadObject(GLObjectContext *ctx,
                    GLFunctions *f,
                    QString const& basePath,
                    QString const& fileName,
                    bool linearSampling,
                    bool anisotropyFilter,
                    std::unique_ptr<VertexArrayObject> &&vao = nullptr,
                    std::unique_ptr<VertexVBO> &&vbo = nullptr);

} // namespace cw

#endif // PROJECT_GL2_WAVEFRONT_LOADER_H
