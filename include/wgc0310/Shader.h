#ifndef PROJECT_WG_WGC0310_SHADER_H
#define PROJECT_WG_WGC0310_SHADER_H

#include "cwglx/Base/ShaderProgram.h"

namespace wgc0310 {

struct ShaderCollection {
  cw::ShaderProgram emissiveShader;
  cw::ShaderProgram strokeShader;

  cw::ShaderProgram opaqueShader;
  cw::ShaderProgram translucentShader;

  void Delete(GLFunctions *f);
};

struct ShaderText {
  QString opaqueVS;
  QString opaqueFS;

  QString translucentVS;
  QString translucentFS;
};

std::unique_ptr<ShaderCollection>
CompileShader(GLFunctions *f, ShaderText const&, QString *err);

ShaderText GetPhongShaderText();

ShaderText GetLegacyPhongShaderText();

ShaderText GetGouraudShaderText();

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SHADER_H
