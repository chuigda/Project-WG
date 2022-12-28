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

std::unique_ptr<ShaderCollection> LoadDefaultShader(GLFunctions *f);

std::unique_ptr<ShaderCollection> LoadGouraudShader(GLFunctions *f);

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SHADER_H
