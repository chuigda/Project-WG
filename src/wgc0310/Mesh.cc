#include "wgc0310/Mesh.h"

#include "GlobalConfig.h"
#include "cwglx/GL/GLImpl.h"
#include "cwglx/Base/Shader.h"
#include "cwglx/Object/WavefrontLoader.h"
#include "util/FileUtil.h"

namespace wgc0310 {

static cw::GLObject
LoadObjectEx(cw::GLObjectContext *ctx, GLFunctions *f, QString const& objectFile) {
  QString patchPath = QStringLiteral("./patch/model/%1").arg(objectFile);
  if (cw::IsFileExists(patchPath)) {
    return cw::LoadObject(
      ctx,
      f,
      "./patch/model",
      objectFile,
      cw::GlobalConfig::Instance.linearSampling,
      cw::GlobalConfig::Instance.anisotropyFilter
    );
  }
  return cw::LoadObject(
    ctx,
    f,
    ":/model",
    objectFile,
    cw::GlobalConfig::Instance.linearSampling,
    cw::GlobalConfig::Instance.anisotropyFilter
  );
}

WGCModel LoadWGCModel(cw::GLObjectContext *ctx, GLFunctions *f) {
  return {
    .testObject = LoadObjectEx(ctx, f, "TestObject.obj")
  };
}

void WGCModel::Delete(GLFunctions *f) {
  testObject.Delete(f);
}

} // namespace wgc0310
