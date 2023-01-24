#ifndef PROJECT_GL2_CWGLX_SHADER_PROGRAM_H
#define PROJECT_GL2_CWGLX_SHADER_PROGRAM_H

#include <QString>
#include "include/cwglx/GL/GL.h"
#include "util/Derive.h"

namespace cw {

class Shader {
public:
  Shader(QString programText, GLenum shaderType);

  ~Shader();

  bool Compile(GLFunctions *f);

  void Delete(GLFunctions *f);

  [[nodiscard]] QString GetCompileError() const;

  [[nodiscard]] bool IsReady() const;

  [[nodiscard]] GLuint GetShaderId() const;

  CW_DERIVE_UNCOPYABLE(Shader);
  CW_DERIVE_UNMOVABLE(Shader);

private:
  QString m_ProgramText;
  GLenum m_ShaderType;
  GLuint m_ShaderId;

  bool m_Compiled;
  QString m_CompileError;
};

} // namespace cw

#endif // PROJECT_GL2_CWGLX_SHADER_PROGRAM_H
