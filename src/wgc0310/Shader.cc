#include "wgc0310/Shader.h"

#include <QDebug>
#include "cwglx/Base/Shader.h"
#include "util/FileUtil.h"

namespace wgc0310 {

void wgc0310::ShaderCollection::Delete(GLFunctions *f) {
  emissiveShader.Delete(f);
  translucentShader.Delete(f);
  opaqueShader.Delete(f);
  strokeShader.Delete(f);
}

static bool CompileShaderPair(GLFunctions *f,
                              cw::ShaderProgram *program,
                              QString const& vsPath,
                              QString const& fsPath)
{
  auto critical = qCritical().noquote();

  program->InitCompilation(f);

  cw::Shader vertexShader { cw::ReadToString(vsPath), GL_VERTEX_SHADER };
  cw::Shader fragmentShader { cw::ReadToString(fsPath), GL_FRAGMENT_SHADER };

  bool success = false;
  if (!vertexShader.Compile(f)) {
    critical << "wgc0310::CompileShaderPair(...):"
             << "error compiling vertex shader"
             << vsPath
             << ":"
             << vertexShader.GetCompileError();
    goto cleanup;
  }

  if (!fragmentShader.Compile(f)) {
    critical << "wgc0310::CompileShaderPair(...):"
             << "error compiling fragment shader"
             << fsPath
             << ":"
             << fragmentShader.GetCompileError();
    goto cleanup;
  }

  program->AttachShader(f, &vertexShader);
  program->AttachShader(f, &fragmentShader);
  if (!program->Link(f)) {
    critical << "wgc0310::CompileShaderPair(...):"
             << "error linking shader program"
             << vsPath
             << "and"
             << fsPath
             << ":"
             << program->GetCompileError();
    goto cleanup;
  }

  success = true;
cleanup:
  vertexShader.Delete(f);
  fragmentShader.Delete(f);
  return success;
}

std::unique_ptr<ShaderCollection> LoadDefaultShader(GLFunctions *f) {
  auto ret = std::make_unique<ShaderCollection>();
  if (!CompileShaderPair(f, &ret->emissiveShader,
                         ":/shader/common/emissive.vert",
                         ":/shader/common/emissive.frag")) {
    ret->Delete(f);
    return nullptr;
  }

  if (!CompileShaderPair(f, &ret->strokeShader,
                         ":/shader/common/stroke.vert",
                         ":/shader/common/stroke.frag")) {
    ret->Delete(f);
    return nullptr;
  }

  if (!CompileShaderPair(f, &ret->opaqueShader,
                         ":/shader/phong/opaque.vert",
                         ":/shader/phong/opaque.frag")) {
    ret->Delete(f);
    return nullptr;
  }

  if (!CompileShaderPair(f, &ret->translucentShader,
                         ":/shader/phong/translucent.vert",
                         ":/shader/phong/translucent.frag")) {
    ret->Delete(f);
    return nullptr;
  }

  return ret;
}

std::unique_ptr<ShaderCollection> LoadGouraudShader(GLFunctions *f) {
  auto ret = std::make_unique<ShaderCollection>();
  if (!CompileShaderPair(f, &ret->emissiveShader,
                         ":/shader/common/emissive.vert",
                         ":/shader/common/emissive.frag")) {
    ret->Delete(f);
    return nullptr;
  }

  if (!CompileShaderPair(f, &ret->strokeShader,
                         ":/shader/common/stroke.vert",
                         ":/shader/common/stroke.frag")) {
    ret->Delete(f);
    return nullptr;
  }

  if (!CompileShaderPair(f, &ret->opaqueShader,
                         ":/shader/gouraud/opaque.vert",
                         ":/shader/gouraud/opaque.frag")) {
    ret->Delete(f);
    return nullptr;
  }

  if (!CompileShaderPair(f, &ret->translucentShader,
                         ":/shader/phong/translucent.vert",
                         ":/shader/phong/translucent.frag")) {
    ret->Delete(f);
    return nullptr;
  }

  return ret;
}

} // namespace wgc0310
