#include "wgc0310/Mesh.h"

#include "cwglx/GL/GLImpl.h"
#include "cwglx/Base/Shader.h"
#include "cwglx/Object/WavefrontLoader.h"

namespace wgc0310 {

WGCMeshCollection LoadWGCMesh(cw::GLObjectContext *ctx, GLFunctions *f) {
  return {
    .testObject = cw::LoadObject(ctx, f, ":/model", "TestObject.obj")
  };
}

void WGCMeshCollection::Delete(GLFunctions *f) {
  testObject.Delete(f);
}

} // namespace wgc0310
