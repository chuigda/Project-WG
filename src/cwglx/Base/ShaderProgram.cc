#include <glm/gtc/type_ptr.hpp>
#include "include/cwglx/Base/ShaderProgram.h"
#include "include/cwglx/GL/GLImpl.h"
#include "include/cwglx/Base/Shader.h"

namespace cw {

ShaderProgram::ShaderProgram()
  : m_ProgramId(0),
    m_Initialised(false),
    m_Linked(false)
{}

ShaderProgram::~ShaderProgram() {
  if (m_Initialised) {
    qWarning() << "ShaderProgram::~ShaderProgram():"
               << "shader program deleted before releasing relevant OpenGL resources";
  }
}

void ShaderProgram::InitCompilation(GLFunctions *f) {
  if (m_Initialised) {
    return;
  }

  m_ProgramId = f->glCreateProgram();
  m_Initialised = true;
  m_CompileError.clear();
}

void ShaderProgram::AttachShader(GLFunctions *f, Shader *shader) {  // NOLINT(readability-make-member-function-const)
  if (!m_Initialised) {
    qWarning() << "ShaderProgram::AttachShader(GLFunctions*, Shader*):"
               << "compilation stage must be initialised first";
    return;
  }

  if (m_Linked) {
    qWarning() << "ShaderProgram::AttachShader(GLFunctions*, Shader*):"
               << "linked shader program cannot be modified";
    return;
  }

  if (!shader->IsReady()) {
    qWarning() << "ShaderProgram::AttachShader(GLFunctions*, Shader*):"
               << "cannot attach a not compiled shader";
    return;
  }

  f->glAttachShader(m_ProgramId, shader->GetShaderId());
}

bool ShaderProgram::Link(GLFunctions *f) {
  if (!m_Initialised) {
    qWarning() << "ShaderProgram::Link(GLFunctions*):"
               << "compilation stage must be initialised first";
    return false;
  }

  if (m_Linked) {
    qWarning() << "ShaderProgram::Link(GLFunctions*):"
               << "linked shader program cannot be modified";
    return false;
  }

  f->glLinkProgram(m_ProgramId);
  GLint success;
  f->glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &success);
  if (!success) {
    GLint logLength;
    f->glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &logLength);

    char *buffer = new char[logLength + 1];
    f->glGetProgramInfoLog(m_ProgramId, logLength, nullptr, buffer);
    m_CompileError = QString::fromUtf8(buffer);
    delete[] buffer;

    f->glDeleteProgram(m_ProgramId);
    m_Initialised = false;
    return false;
  }

  m_Linked = true;
  return true;
}

void ShaderProgram::UseProgram(GLFunctions *f) const {
  if (!m_Linked) {
    qWarning() << "ShaderProgram::UseProgram(GLFunctions*):"
               << "cannot activate a not linked program";
  }
  f->glUseProgram(m_ProgramId);
}

void ShaderProgram::Delete(GLFunctions *f) {
  if (m_Initialised) {
    f->glDeleteProgram(m_ProgramId);
    m_CachedUniforms.clear();
    m_Initialised = false;
    m_Linked = false;
  }
}

QString ShaderProgram::GetCompileError() const {
  return m_CompileError;
}

GLint ShaderProgram::GetUniformLocation(GLFunctions *f,
                                        QString const& uniformName) const {
  auto it = m_CachedUniforms.find(uniformName);
  if (it != m_CachedUniforms.end()) {
    return *it;
  }

  GLint location = f->glGetUniformLocation(m_ProgramId, uniformName.toStdString().c_str());
  if (location < 0) {
    qCritical() << "ShaderProgram::GetUniformLocation(GLFunctions*, QString const&):"
                << "error retrieving uniform location of"
                << uniformName
                << ", glGetError() ="
                << f->glGetError();
  }
  m_CachedUniforms.insert(uniformName, location);
  return location;
}

void ShaderProgram::SetUniform(GLFunctions *f,
                               const QString &uniformName,
                               GLint value) {
  GLint uniformLocation = GetUniformLocation(f, uniformName);
  if (uniformLocation < 0) {
    return;
  }

  f->glUniform1i(uniformLocation, value);
  qDebug() << "glGetError() after setting" << uniformName << "=" << f->glGetError();
}

void ShaderProgram::SetUniform(GLFunctions *f,
                               const QString &uniformName,
                               GLuint value) {
  GLint uniformLocation = GetUniformLocation(f, uniformName);
  if (uniformLocation < 0) {
    return;
  }

  f->glUniform1ui(uniformLocation, value);
}

void ShaderProgram::SetUniform(GLFunctions *f,
                               const QString &uniformName,
                               GLfloat value) {
  GLint uniformLocation = GetUniformLocation(f, uniformName);
  if (uniformLocation < 0) {
    return;
  }

  f->glUniform1f(uniformLocation, value);
}

void ShaderProgram::SetUniform(GLFunctions *f,
                               const QString &uniformName,
                               const glm::vec3 &value) {
  SetUniform3fv(f, uniformName, glm::value_ptr(value));
}

void ShaderProgram::SetUniform(GLFunctions *f,
                               const QString &uniformName,
                               const glm::vec4 &value) {
  SetUniform4fv(f, uniformName, glm::value_ptr(value));
}

void ShaderProgram::SetUniform(GLFunctions *f,
                               const QString &uniformName,
                               const glm::mat4 &value) {
  SetUniformMatrix4fv(f, uniformName, glm::value_ptr(value));
}

void ShaderProgram::SetUniform4fv(GLFunctions *f,
                                  const QString &uniformName,
                                  GLfloat const* value) {
  GLint uniformLocation = GetUniformLocation(f, uniformName);
  if (uniformLocation < 0) {
    return;
  }

  f->glUniform4fv(uniformLocation, 1, value);
}

void ShaderProgram::SetUniform3fv(GLFunctions *f,
                                  const QString &uniformName,
                                  const GLfloat *value) {
  GLint uniformLocation = GetUniformLocation(f, uniformName);
  if (uniformLocation < 0) {
    return;
  }

  f->glUniform3fv(uniformLocation, 1, value);
}

void ShaderProgram::SetUniformMatrix4fv(GLFunctions *f,
                                        const QString &uniformName,
                                        GLfloat const* value) {
  GLint uniformLocation = GetUniformLocation(f, uniformName);
  if (uniformLocation < 0) {
    return;
  }

  f->glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, value);
}

} // namespace cw
