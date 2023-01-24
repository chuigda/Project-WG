#ifndef PROJECT_GL2_SHADER_PROGRAM_H
#define PROJECT_GL2_SHADER_PROGRAM_H

#include <QString>
#include <QMap>
#include <glm/fwd.hpp>
#include "include/cwglx/GL/GL.h"
#include "util/Derive.h"

namespace cw {

class Shader;

class ShaderProgram {
public:
  ShaderProgram();

  ~ShaderProgram();

  void InitCompilation(GLFunctions *f);

  void AttachShader(GLFunctions *f, Shader *shader);

  bool Link(GLFunctions *f);

  void UseProgram(GLFunctions *f) const;

  void Delete(GLFunctions *f);

  [[nodiscard]] QString GetCompileError() const;

  GLint GetUniformLocation(GLFunctions *f, QString const& uniformName) const;

  void SetUniform(GLFunctions *f, QString const& uniformName, GLint value);

  void SetUniform(GLFunctions *f, QString const& uniformName, GLuint value);

  void SetUniform(GLFunctions *f, QString const& uniformName, GLfloat value);

  void SetUniform(GLFunctions *f, QString const& uniformName, glm::vec3 const& value);

  void SetUniform(GLFunctions *f, QString const& uniformName, glm::vec4 const& value);

  void SetUniform(GLFunctions *f, QString const& uniformName, glm::mat4 const& value);

  void SetUniform3fv(GLFunctions *f, QString const& uniformName, GLfloat const* value);

  void SetUniform4fv(GLFunctions *f, QString const& uniformName, GLfloat const* value);

  void SetUniformMatrix4fv(GLFunctions *f, QString const& uniformName, GLfloat const* value);

  CW_DERIVE_UNCOPYABLE(ShaderProgram)
  CW_DERIVE_UNMOVABLE(ShaderProgram)

private:
  GLuint m_ProgramId;

  bool m_Initialised;
  bool m_Linked;
  QString m_CompileError;

  mutable QMap<QString, GLint> m_CachedUniforms;
};

} // namespace cw

#endif // PROJECT_GL2_SHADER_PROGRAM_H
