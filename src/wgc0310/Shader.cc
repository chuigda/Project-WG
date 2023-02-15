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
                              QString const& role,
                              QString const& vs,
                              QString const& fs,
                              QString *err = nullptr)
{
  auto critical = qCritical().noquote();

  program->InitCompilation(f);

  cw::Shader vertexShader { vs, GL_VERTEX_SHADER };
  cw::Shader fragmentShader { fs, GL_FRAGMENT_SHADER };

  bool success = false;
  if (!vertexShader.Compile(f)) {
    critical << "wgc0310::CompileShaderPair(...):"
             << "error compiling vertex shader for"
             << role
             << ":"
             << vertexShader.GetCompileError();
    if (err) {
      *err = QStringLiteral("用于%1的顶点着色器编译失败:\r\n%2")
        .arg(role)
        .arg(vertexShader.GetCompileError());
    }
    goto cleanup;
  }

  if (!fragmentShader.Compile(f)) {
    critical << "wgc0310::CompileShaderPair(...):"
             << "error compiling fragment shader for"
             << role
             << ":"
             << fragmentShader.GetCompileError();
    if (err) {
      *err = QStringLiteral("用于%1的片段着色器编译失败:\r\n%2")
        .arg(role)
        .arg(fragmentShader.GetCompileError());
    }
    goto cleanup;
  }

  program->AttachShader(f, &vertexShader);
  program->AttachShader(f, &fragmentShader);
  if (!program->Link(f)) {
    critical << "wgc0310::CompileShaderPair(...):"
             << "error linking shader program for"
             << role
             << ":"
             << program->GetCompileError();
    if (err) {
      *err = QStringLiteral("用于%1的着色器程序链接失败:\r\n%2")
        .arg(role)
        .arg(program->GetCompileError());
    }
    goto cleanup;
  }

  success = true;
cleanup:
  vertexShader.Delete(f);
  fragmentShader.Delete(f);
  return success;
}

static bool CompileCommonShader(GLFunctions *f, ShaderCollection *c, QString *err = nullptr) {
  auto ret = std::make_unique<ShaderCollection>();
  if (!CompileShaderPair(f, &c->emissiveShader, QStringLiteral("发光体"),
                         cw::ReadToString(QStringLiteral(":/shader/common/emissive.vert")),
                         cw::ReadToString(QStringLiteral(":/shader/common/emissive.frag")),
                         err))
  {
    ret->Delete(f);
    return false;
  }

  if (!CompileShaderPair(f, &c->strokeShader, QStringLiteral("边线"),
                         cw::ReadToString(QStringLiteral(":/shader/common/stroke.vert")),
                         cw::ReadToString(QStringLiteral(":/shader/common/stroke.frag")),
                         err))
  {
    ret->Delete(f);
    return false;
  }

  return true;
}

std::unique_ptr<ShaderCollection>
CompileShader(GLFunctions *f, ShaderText const& text, QString *err) {
  auto ret = std::make_unique<ShaderCollection>();
  if (!CompileCommonShader(f, ret.get(), err)) {
    return nullptr;
  }

  if (!CompileShaderPair(f, &ret->opaqueShader, QStringLiteral("不透明体"),
                         text.opaqueVS, text.opaqueFS, err))
  {
    ret->Delete(f);
    return nullptr;
  }

  if (!CompileShaderPair(f, &ret->translucentShader, QStringLiteral("半透明体"),
                         text.translucentVS, text.translucentFS, err))
  {
    ret->Delete(f);
    return nullptr;
  }

  return ret;
}

ShaderText GetPhongShaderText() {
  return ShaderText {
    .opaqueVS = cw::ReadToString(QStringLiteral(":/shader/standard/opaque.vert")),
    .opaqueFS = cw::ReadToString(QStringLiteral(":/shader/standard/opaque.frag")),
    .translucentVS = cw::ReadToString(QStringLiteral(":/shader/standard/translucent.vert")),
    .translucentFS = cw::ReadToString(QStringLiteral(":/shader/standard/translucent.frag"))
  };
}

ShaderText GetLegacyPhongShaderText() {
  return ShaderText {
    .opaqueVS = cw::ReadToString(QStringLiteral(":/shader/standard/opaque.vert")),
    .opaqueFS = cw::ReadToString(QStringLiteral(":/shader/standard/opaque.legacy.frag")),
    .translucentVS = cw::ReadToString(QStringLiteral(":/shader/standard/translucent.vert")),
    .translucentFS = cw::ReadToString(QStringLiteral(":/shader/standard/translucent.frag"))
  };
}

ShaderText GetGouraudShaderText() {
  return ShaderText {
    .opaqueVS = cw::ReadToString(QStringLiteral(":/shader/gouraud/opaque.vert")),
    .opaqueFS = cw::ReadToString(QStringLiteral(":/shader/gouraud/opaque.frag")),
    .translucentVS = cw::ReadToString(QStringLiteral(":/shader/gouraud/translucent.vert")),
    .translucentFS = cw::ReadToString(QStringLiteral(":/shader/gouraud/translucent.frag"))
  };
}

} // namespace wgc0310
