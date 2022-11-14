#include "include/wgc0310/Screen.h"

#include <cstdlib>
#include <QThread>
#include <QDebug>
#include <QImage>
#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"
#include "include/wgc0310/ScreenCurveHelper.h"
#include "util/Derive.h"

namespace wgc0310 {

class ScreenImpl {
public:
  explicit ScreenImpl(GLFunctions *f);
  ~ScreenImpl();

  std::vector<cw::VertexF> screenVertices;
  std::vector<cw::Vertex2DF> screenTexCoords;
  std::vector<GLuint> screenIndices;

  bool deleted;
  std::array<GLuint, 3> vbo;
  std::array<GLuint, 3> vbo2D;

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
ScreenImpl::ScreenImpl(GLFunctions *f) {
  Initialize3D(f);
  InitializeTexture(f);

  deleted = false;
}
#pragma clang diagnostic pop

ScreenImpl::~ScreenImpl() {
  if (!deleted) {
    qWarning() << "ScreenImpl::~ScreenImpl():"
               << "VBO not deleted";
  }
}

void ScreenImpl::Initialize3D(GLFunctions *f) {
  std::vector<std::vector<cw::Vertex>> screenVertices_ =
      ComputeScreenVertices(20.0, 15.0, 1.25, 160, 120);
  for (int y = 0; y <= 120; y++) {
    for (int x = 0; x <= 160; x++) {
      screenVertices.push_back(cw::VertexF::Downscale(screenVertices_[y][x]));

      GLfloat texX = static_cast<GLfloat>(x) / 160.0f;
      GLfloat texY = static_cast<GLfloat>(y) / 120.0f;
      screenTexCoords.emplace_back(texX, texY);
    }
  }

  for (int y = 0; y < 120; y ++) {
    for (int x = 0; x < 160; x ++) {
      int pointA = x + y * 161;
      int pointB = x + (y + 1) * 161;
      int pointC = (x + 1) + (y + 1) * 161;
      int pointD = (x + 1) + y * 161;

      screenIndices.push_back(static_cast<GLuint>(pointA));
      screenIndices.push_back(static_cast<GLuint>(pointB));
      screenIndices.push_back(static_cast<GLuint>(pointC));

      screenIndices.push_back(static_cast<GLuint>(pointA));
      screenIndices.push_back(static_cast<GLuint>(pointC));
      screenIndices.push_back(static_cast<GLuint>(pointD));
    }
  }

  // initialize VBO for 3D things
  f->glGenBuffers(3, vbo.data());

  // 0 for vertices
  f->glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  f->glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizei>(sizeof(cw::VertexF) * screenVertices.size()),
      screenVertices.data(),
      GL_STATIC_DRAW
  );

  // 1 for indices
  f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
  f->glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      static_cast<GLsizei>(sizeof(GLuint) * screenIndices.size()),
      screenIndices.data(),
      GL_STATIC_DRAW
  );

  // 2 for texture coordinates
  f->glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  f->glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizei>(sizeof(cw::Vertex2DF) * screenTexCoords.size()),
      screenTexCoords.data(),
      GL_STATIC_DRAW
  );
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

  if(f->glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    qCritical() << "ScreenImpl::InitializeTexture:"
                << "failed initializing frame buffer object, glGetError() ="
                << f->glGetError();
    std::abort();
  }
}

void ScreenImpl::Delete(GLFunctions *f) {
  if (!deleted) {
    f->glDeleteBuffers(3, vbo.data());
    f->glDeleteTextures(1, &screenTextureId);
    f->glDeleteFramebuffers(1, &fbo);
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

  f->glPushAttrib(GL_ALL_ATTRIB_BITS);
  f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

  f->glViewport(0, 0, 640, 480);
  f->glMatrixMode(GL_PROJECTION);
  f->glLoadIdentity();
  f->glOrtho(-320, 320, -240, 240, -1, 1);

  f->glMatrixMode(GL_MODELVIEW);
  f->glLoadIdentity();

  f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  f->glClear(GL_COLOR_BUFFER_BIT);
  f->glTranslatef(0.0f, 0.0f, -0.5f);

  f->glDisable(GL_LIGHTING);
  f->glDisable(GL_MULTISAMPLE);
  f->glDisable(GL_DEPTH_TEST);
}

void Screen::DoneScreenContext(GLFunctions *f) const noexcept {
  f->glPopClientAttrib();
  f->glPopAttrib();

  // no need to restore frame buffer here, we'll do that somewhere else
}

void Screen::Draw(GLFunctions *f) const noexcept {
  f->glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT);
  f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

  f->glDisable(GL_LIGHTING);

  // set color to pure white
  f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  // use the texture we've drawn
  f->glEnable(GL_TEXTURE_2D);
  f->glBindTexture(GL_TEXTURE_2D, m_Impl->screenTextureId);

  // bind our vbo
  f->glEnableClientState(GL_VERTEX_ARRAY);
  f->glBindBuffer(GL_ARRAY_BUFFER, m_Impl->vbo[0]);
  f->glVertexPointer(3, GL_FLOAT, 0, nullptr);

  f->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  f->glBindBuffer(GL_ARRAY_BUFFER, m_Impl->vbo[2]);
  f->glTexCoordPointer(2, GL_FLOAT, 0, nullptr);

  // draw the screen
  f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Impl->vbo[1]);
  f->glDrawElements(GL_TRIANGLES,
                    static_cast<GLsizei>(m_Impl->screenIndices.size()),
                    GL_UNSIGNED_INT,
                    nullptr);

  // restore states
  f->glPopClientAttrib();
  f->glPopAttrib();
}

void Screen::Delete(GLFunctions *f) const noexcept {
  m_Impl->Delete(f);
}

} // namespace wgc0310
