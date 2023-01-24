#include "include/wgc0310/Screen.h"

#include <cstdlib>
#include <QImage>
#include <glm/vec2.hpp>
#include "cwglx/GL/GLImpl.h"
#include "cwglx/Base/VertexArrayObject.h"
#include "cwglx/Base/VertexBufferObject.h"
#include "cwglx/Base/VertexBufferObjectImpl.h"
#include "cwglx/Base/ElementBufferObject.h"
#include "cwglx/Base/VBOImpl/GLM.h"
#include "cwglx/Base/Shader.h"
#include "cwglx/Base/ShaderProgram.h"
#include "wgc0310/ScreenCurveHelper.h"
#include "util/Derive.h"
#include "util/FileUtil.h"

namespace wgc0310 {

struct ScreenVertex {
  glm::vec3 vertexCoord;
  glm::vec2 texCoord;
};

using ScreenVertexVBO = CW_DEFINE_VBO_TYPE(ScreenVertex, vertexCoord, texCoord);

class ScreenImpl {
public:
  explicit ScreenImpl(GLFunctions *f);
  ~ScreenImpl();

  bool deleted;

  std::unique_ptr<cw::VertexArrayObject> vao;
  std::unique_ptr<ScreenVertexVBO> vbo;
  std::unique_ptr<cw::ElementBufferObject> ebo;

  GLuint fbo;
  GLuint screenTextureId;

  CW_DERIVE_UNCOPYABLE(ScreenImpl)
  CW_DERIVE_UNMOVABLE(ScreenImpl)

  void Delete(GLFunctions *f);

private:
  void Initialize3D(GLFunctions *f);
  void InitializeTexture(GLFunctions *f);
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
ScreenImpl::ScreenImpl(GLFunctions *f)
  : deleted(false)
{
  Initialize3D(f);
  InitializeTexture(f);
}
#pragma clang diagnostic pop

ScreenImpl::~ScreenImpl() {
  if (!deleted) {
    qWarning() << "ScreenImpl::~ScreenImpl():"
               << "VBO not deleted";
  }
}

void ScreenImpl::Initialize3D(GLFunctions *f) {
  std::vector<std::vector<glm::vec3>> screenVertices =
      ComputeScreenVertices(25.0, 18.75, 1.25, 160, 120);

  std::vector<ScreenVertex> vertices;
  vertices.reserve(160 * 120);
  for (int y = 0; y <= 120; y++) {
    for (int x = 0; x <= 160; x++) {
      GLfloat u = static_cast<GLfloat>(x) / 160.0f;
      GLfloat v = static_cast<GLfloat>(y) / 120.0f;

      vertices.push_back(ScreenVertex {
        .vertexCoord = screenVertices[y][x],
        .texCoord = glm::vec2 { u, v },
      });
    }
  }

  std::vector<GLuint> indices;
  indices.reserve(160 * 120 * 2);
  for (int y = 0; y < 120; y ++) {
    for (int x = 0; x < 160; x ++) {
      int pointA = x + y * 161;
      int pointB = x + (y + 1) * 161;
      int pointC = (x + 1) + (y + 1) * 161;
      int pointD = (x + 1) + y * 161;

      indices.push_back(static_cast<GLuint>(pointA));
      indices.push_back(static_cast<GLuint>(pointB));
      indices.push_back(static_cast<GLuint>(pointC));

      indices.push_back(static_cast<GLuint>(pointA));
      indices.push_back(static_cast<GLuint>(pointC));
      indices.push_back(static_cast<GLuint>(pointD));
    }
  }

  vao = std::make_unique<cw::VertexArrayObject>(f);
  vao->Bind(f);

  vbo = std::make_unique<ScreenVertexVBO>(f);
  vbo->Bind(f);
  vbo->BufferData(f, vertices.data(), vertices.size());

  ebo = std::make_unique<cw::ElementBufferObject>(f);
  ebo->Bind(f);
  ebo->BufferData(f, indices.data(), indices.size());

  vao->Unbind(f);
  vbo->Unbind(f);
  ebo->Unbind(f);
}

void ScreenImpl::InitializeTexture(GLFunctions *f) {
  // initialize FBO first
  f->glGenFramebuffers(1, &fbo);
  f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  f->glGenTextures(1, &screenTextureId);
  f->glBindTexture(GL_TEXTURE_2D, screenTextureId);
  f->glTexImage2D(GL_TEXTURE_2D,
                  0,
                  GL_RGB,
                  640,
                  480,
                  0,
                  GL_RGB,
                  GL_UNSIGNED_BYTE,
                  GL_FALSE);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  f->glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screenTextureId, 0);
  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  f->glDrawBuffers(1, DrawBuffers);

  GLenum bufferStatus = f->glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (bufferStatus != GL_FRAMEBUFFER_COMPLETE) {
    qCritical() << "ScreenImpl::InitializeTexture:"
                << "failed initializing frame buffer object, glGetError() ="
                << f->glGetError();
    std::abort();
  }
}

void ScreenImpl::Delete(GLFunctions *f) {
  if (!deleted) {
    f->glDeleteTextures(1, &screenTextureId);
    f->glDeleteFramebuffers(1, &fbo);

    vao->Delete(f);
    vbo->Delete(f);
    ebo->Delete(f);

    deleted = true;
  }
}

Screen::Screen(GLFunctions *f)
  : m_Impl(new ScreenImpl(f))
{}

Screen::~Screen() {
  delete m_Impl;
}

void Screen::BeginScreenContext(GLFunctions *f) const noexcept {
  f->glBindFramebuffer(GL_FRAMEBUFFER, m_Impl->fbo);
  f->glDisable(GL_DEPTH_TEST);
}

void Screen::DoneScreenContext(GLFunctions *f) const noexcept {
  Q_UNUSED(this)
  f->glEnable(GL_DEPTH_TEST);
  // no need to restore frame buffer here, we'll do that somewhere else
}

void Screen::Draw(GLFunctions *f, cw::ShaderProgram *shaderProgram) const noexcept {
  f->glActiveTexture(GL_TEXTURE0);
  f->glBindTexture(GL_TEXTURE_2D, m_Impl->screenTextureId);
  shaderProgram->SetUniform(f, QStringLiteral("screenTexture"), 0);

  m_Impl->vao->Bind(f);
  f->glDrawElements(GL_TRIANGLES, 120 * 160 * 6, GL_UNSIGNED_INT, nullptr);
  m_Impl->vao->Unbind(f);
}

void Screen::Delete(GLFunctions *f) const noexcept {
  m_Impl->Delete(f);
}

} // namespace wgc0310
