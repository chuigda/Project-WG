#ifndef PROJECT_WG_WGC0310_MESH_H
#define PROJECT_WG_WGC0310_MESH_H

#include <cstdint>
#include <memory>

#include "cwglx/GL/GL.h"
#include "cwglx/Object/Vertex.h"
#include "cwglx/Base/VertexArrayObject.h"
#include "cwglx/Base/ShaderProgram.h"
#include "cwglx/Object/Object.h"

namespace wgc0310 {

struct WGCMeshCollection {
  cw::GLObject testObject;

  void Delete(GLFunctions *f);
};

WGCMeshCollection LoadWGCMesh(cw::GLObjectContext *ctx, GLFunctions *f);

} // namespace wgc0310

#endif /* PROJECT_WG_WGC0310_MESH_H */
