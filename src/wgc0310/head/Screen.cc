#include "wgc0310/head/Screen.h"

#include <QDebug>
#include <QImage>
#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"
#include "wgc0310/head/ScreenCurveHelper.h"
#include "util/Derive.h"

namespace wgc0310 {

class ScreenImpl {
public:
  ScreenImpl(GLFunctions *f, const QImage &volumeBarImage);
  ~ScreenImpl();

  cw::Texture2D volumeBarTexture;

  GLuint screenTextureId;

  std::array<double, 10> volumeLevels;

  std::vector<cw::Vertex2DF> volumeBarVertices;
  std::vector<cw::Vertex2DF> volumeBarTexCoords;
  std::vector<GLuint> volumeBarIndices;

  std::vector<cw::VertexF> screenVertices;
  std::vector<cw::Vertex2DF> screenTexCoords;
  std::vector<GLuint> screenIndices;

  bool deleted;
  std::array<GLuint, 3> vbo;

  CW_DERIVE_UNCOPYABLE(ScreenImpl)
  CW_DERIVE_UNMOVABLE(ScreenImpl)

  void Delete(GLFunctions *f);

private:
  void Initialize3D(GLFunctions *f);
  void Initialize2D();
  void InitializeTexture(GLFunctions *f);
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
ScreenImpl::ScreenImpl(GLFunctions *f, const QImage &volumeBarImage)
    : volumeBarTexture(volumeBarImage, f),
      volumeLevels { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
{
  Initialize3D(f);
  Initialize2D();
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
  std::vector<std::vector<cw::Vertex>> screenVertices_ =
      ComputeScreenVertices(22.0, 14.0, 1.5, 160, 120);
  for (int y = 0; y < 120; y++) {
    for (int x = 0; x < 160; x++) {
      screenVertices.push_back(cw::VertexF::Downscale(screenVertices_[y][x]));

      GLfloat texX = static_cast<GLfloat>(x) / 160.0f;
      GLfloat texY = static_cast<GLfloat>(y) / 120.0f;
      screenTexCoords.emplace_back(texX, texY);
    }
  }

  for (int x = 0; x < 159; x ++) {
    for (int y = 0; y < 119; y ++) {
      int pointA = x + y * 160;
      int pointB = x + (y + 1) * 160;
      int pointC = (x + 1) + (y + 1) * 160;
      int pointD = (x + 1) + y * 160;

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
      static_cast<GLsizei>(sizeof(cw::VertexF) * screenTexCoords.size()),
      screenTexCoords.data(),
      GL_STATIC_DRAW
  );
  deleted = false;
}

void ScreenImpl::Initialize2D() {
  GLfloat y0 = 16.0f - 240.0f;
  GLfloat y1 = 24.0f - 240.0f;
  GLfloat y2 = 24.0f + 432.0f - 240.0f;
  GLfloat y3 = 24.0f + 432.0f + 8.0f - 240.0f;

  cw::Vertex2DF texPointA { 0.0f, 0.0f };
  cw::Vertex2DF texPointB { 1.0f, 0.0f };
  cw::Vertex2DF texPointC { 1.0f, 0.25f };
  cw::Vertex2DF texPointD { 0.0f, 0.25f };
  cw::Vertex2DF texPointE { 0.0f, 0.75f };
  cw::Vertex2DF texPointF { 1.0f, 0.75f };
  cw::Vertex2DF texPointG { 1.0f, 1.0f };
  cw::Vertex2DF texPointH { 0.0f, 1.0f };

  for (int i = 0; i < 10; i++) {
    GLfloat fi = static_cast<GLfloat>(i);
    GLfloat x0 = 22.0f + 4.0f * fi - 320.0f;
    GLfloat x1 = 22.0f + 4.0f * fi + 56.0f * fi - 320.0f;

    // vertices
    cw::Vertex2DF pointA { x0, y0 };
    cw::Vertex2DF pointB { x1, y0 };
    cw::Vertex2DF pointC { x1, y1 };
    cw::Vertex2DF pointD { x0, y1 };
    cw::Vertex2DF pointE { x0, y2 };
    cw::Vertex2DF pointF { x1, y2 };
    cw::Vertex2DF pointG { x1, y3 };
    cw::Vertex2DF pointH { x0, y3 };

    volumeBarVertices.push_back(pointA);
    volumeBarVertices.push_back(pointB);
    volumeBarVertices.push_back(pointC);
    volumeBarVertices.push_back(pointD);
    volumeBarVertices.push_back(pointE);
    volumeBarVertices.push_back(pointF);
    volumeBarVertices.push_back(pointG);
    volumeBarVertices.push_back(pointH);

    volumeBarTexCoords.push_back(texPointA);
    volumeBarTexCoords.push_back(texPointB);
    volumeBarTexCoords.push_back(texPointC);
    volumeBarTexCoords.push_back(texPointD);
    volumeBarTexCoords.push_back(texPointE);
    volumeBarTexCoords.push_back(texPointF);
    volumeBarTexCoords.push_back(texPointG);
    volumeBarTexCoords.push_back(texPointH);

    // ABC
    volumeBarIndices.push_back(i * 8);
    volumeBarIndices.push_back(i * 8 + 1);
    volumeBarIndices.push_back(i * 8 + 2);

    // ACD
    volumeBarIndices.push_back(i * 8);
    volumeBarIndices.push_back(i * 8 + 2);
    volumeBarIndices.push_back(i * 8 + 3);

    // DCF
    volumeBarIndices.push_back(i * 8 + 3);
    volumeBarIndices.push_back(i * 8 + 2);
    volumeBarIndices.push_back(i * 8 + 5);

    // DFE
    volumeBarIndices.push_back(i * 8 + 3);
    volumeBarIndices.push_back(i * 8 + 5);
    volumeBarIndices.push_back(i * 8 + 4);

    // EFG
    volumeBarIndices.push_back(i * 8 + 4);
    volumeBarIndices.push_back(i * 8 + 5);
    volumeBarIndices.push_back(i * 8 + 6);

    // EGH
    volumeBarIndices.push_back(i * 8 + 4);
    volumeBarIndices.push_back(i * 8 + 6);
    volumeBarIndices.push_back(i * 8 + 7);
  }
}

void ScreenImpl::InitializeTexture(GLFunctions *f) {
  f->glPushAttrib(GL_TEXTURE_BIT);
  f->glEnable(GL_TEXTURE_2D);
  f->glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
  f->glGenTextures(1, &screenTextureId);
  f->glBindTexture(GL_TEXTURE_2D, screenTextureId);
  f->glTexImage2D(GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  640,
                  480,
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  nullptr);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  f->glPopAttrib();
}

void ScreenImpl::Delete(GLFunctions *f) {
  if (!deleted) {
    f->glDeleteBuffers(3, vbo.data());
    f->glDeleteTextures(1, &screenTextureId);
    volumeBarTexture.DeleteTexture(f);
    deleted = true;
  }
}

Screen::Screen(GLFunctions *f) {
  QImage image("./resc/volume-bar-composed.bmp");
  if (image.isNull()) {
    qDebug() << "Screen::Screen: Failed to load image!";
    std::abort();
  }

  m_Impl = new ScreenImpl(f, image);
}

Screen::~Screen() {
  delete m_Impl;
}

void Screen::PrepareTexture(GLFunctions *f) const noexcept {
  // save all rendering state before we continue
  f->glPushAttrib(GL_ALL_ATTRIB_BITS);
  f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
  f->glPushMatrix();

  // disable lighting
  f->glDisable(GL_LIGHTING);
  // disable depth testing
  f->glDisable(GL_DEPTH_TEST);
  // disable multisampling
  f->glDisable(GL_MULTISAMPLE);

  // set up the viewport
  f->glViewport(0, 0, 640, 480);
  // set up ortho projection: -320 ~ 320, -240 ~ 240
  f->glMatrixMode(GL_PROJECTION);
  f->glLoadIdentity();
  f->glOrtho(-320, 320, -240, 240, -1, 1);

  // set up the modelview matrix
  f->glMatrixMode(GL_MODELVIEW);
  f->glLoadIdentity();

  // clear screen
  f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  f->glClear(GL_COLOR_BUFFER_BIT);

  // draw a simple triangle for test use
  f->glBegin(GL_TRIANGLES);
  {
    f->glColor3f(1.0f, 0.0f, 0.0f);
    f->glVertex2f(0.0f, 120.0f);

    f->glColor3f(0.0f, 1.0f, 0.0f);
    f->glVertex2f(-160.0f, -120.0f);

    f->glColor3f(0.0f, 0.0f, 1.0f);
    f->glVertex2f(160.0f, -120.0f);
  }
  f->glEnd();

  // copy the framebuffer to the texture
  f->glEnable(GL_TEXTURE_2D);
  f->glBindTexture(GL_TEXTURE_2D, m_Impl->screenTextureId);
  f->glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 640, 480, GL_FALSE);

  // restore all states
  f->glPopMatrix();
  f->glPopClientAttrib();
  f->glPopAttrib();
}

void Screen::Draw(GLFunctions *f) const noexcept {
  // texture should have been prepared before this call

  // save color and texture state
  f->glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT);
  // save all client state
  f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
  // disable material
  f->glDisable(GL_LIGHTING);

  // set color to pure white
  f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  // now use our texture
  // f->glEnable(GL_TEXTURE_2D);
  // f->glBindTexture(GL_TEXTURE_2D, m_Impl->screenTextureId);

  // test with another texture
  f->glEnable(GL_TEXTURE_2D);
  m_Impl->volumeBarTexture.BeginTexture(f);

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
