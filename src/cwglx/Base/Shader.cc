#include "include/cwglx/Base/Shader.h"

#include <utility>
#include "include/cwglx/GL/GLImpl.h"
#include "wgc0310/Shader.h"


namespace cw {

Shader::Shader(QString programText, GLenum shaderType)
  : m_ProgramText(std::move(programText)),
    m_ShaderType(shaderType),
    m_ShaderId(0),
    m_Compiled(false)
{}

Shader::~Shader() {
  if (m_Compiled) {
    qWarning() << "Shader::~Shader():"
               << "shader object deleted before releasing relevant OpenGL resources";
  }
}

bool Shader::Compile(GLFunctions *f) {
  if (m_Compiled) {
    return true;
  }

  std::string programText = m_ProgramText.toStdString();
  const char *rawProgramText = programText.data();

  m_ShaderId = f->glCreateShader(m_ShaderType);
  f->glShaderSource(m_ShaderId, 1, &rawProgramText, nullptr);
  f->glCompileShader(m_ShaderId);

  GLint success;
  f->glGetShaderiv(m_ShaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint logLength;
    f->glGetShaderiv(m_ShaderId, GL_INFO_LOG_LENGTH, &logLength);

    char *buffer = new char[logLength + 1];
    f->glGetShaderInfoLog(m_ShaderId, logLength, nullptr, buffer);
    m_CompileError = QString::fromUtf8(buffer);
    delete[] buffer;

    f->glDeleteShader(m_ShaderId);
    return false;
  }

  m_Compiled = true;
  return true;
}

void Shader::Delete(GLFunctions *f) {
  if (m_Compiled) {
    f->glDeleteShader(m_ShaderId);
    m_Compiled = false;
  }
}

QString Shader::GetCompileError() const {
  return m_CompileError;
}

bool Shader::IsReady() const {
  return m_Compiled;
}

GLuint Shader::GetShaderId() const {
  return m_ShaderId;
}

} // namespace cw
