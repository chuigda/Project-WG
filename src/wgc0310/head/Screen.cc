#include "wgc0310/head/Screen.h"

#include <QOpenGLFunctions_2_0>
#include "cwglx/Texture.h"
#include "wgc0310/head/ScreenCurveHelper.h"
#include "util/Derive.h"

namespace wgc0310 {

class ScreenImpl {
public:
  ScreenImpl(QOpenGLFunctions_2_0 *f, const QImage &volumeBarImage);
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

  bool vboDeleted;
  std::array<GLuint, 3> vbo;

  CW_DERIVE_UNCOPYABLE(ScreenImpl)
  CW_DERIVE_UNMOVABLE(ScreenImpl)

  void DeleteVBO(QOpenGLFunctions_2_0 *f) {
    if (!vboDeleted) {
      f->glDeleteBuffers(3, vbo.data());
      vboDeleted = true;
    }
  }

private:
  void Initialize3D(QOpenGLFunctions_2_0 *f);
  void Initialize2D();
};

ScreenImpl::ScreenImpl(QOpenGLFunctions_2_0 *f, const QImage &volumeBarImage)
    : volumeBarTexture(volumeBarImage, f),
      volumeLevels { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
{
  Initialize3D(f);
  Initialize2D();
}

ScreenImpl::~ScreenImpl() {
  if (!vboDeleted) {
    qWarning() << "ScreenImpl::~ScreenImpl():"
               << "VBO not deleted";
  }
}

void ScreenImpl::Initialize3D(QOpenGLFunctions_2_0 *f) {
  std::vector<std::vector<cw::Vertex>> screenVertices_ =
      ComputeScreenVertices(22.0, 14.0, 1.5, 160, 120);
  for (std::size_t x = 0; x < 160; x++) {
    for (std::size_t y = 0; y < 120; y++) {
      screenVertices.push_back(cw::VertexF::Downscale(screenVertices_[y][x]));
    }
  }

  for (int x = 0; x < 160; x += 2) {
    for (int y = 0; y < 120; y += 2) {
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

      GLfloat texX1 = static_cast<GLfloat>(x - 80) / 160.0f;
      GLfloat texY1 = static_cast<GLfloat>(y - 60) / 120.0f;
      GLfloat texX2 = static_cast<GLfloat>(x + 1 - 80) / 160.0f;
      GLfloat texY2 = static_cast<GLfloat>(y + 1 - 60) / 120.0f;

      screenTexCoords.emplace_back(texX1, texY1);
      screenTexCoords.emplace_back(texX1, texY2);
      screenTexCoords.emplace_back(texX2, texY2);
      screenTexCoords.emplace_back(texX2, texY1);
    }
  }

  // initialize VBO for 3D things
  f->glGenBuffers(3, vbo.data());

  f->glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  f->glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizei>(sizeof(cw::VertexF) * screenVertices.size()),
      screenVertices.data(),
      GL_STATIC_DRAW
  );

  f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
  f->glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      static_cast<GLsizei>(sizeof(GLuint) * screenIndices.size()),
      screenIndices.data(),
      GL_STATIC_DRAW
  );

  // texture buffer
  f->glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  f->glBufferData(
      GL_ARRAY_BUFFER,
      static_cast<GLsizei>(sizeof(GLfloat) * screenTexCoords.size()),
      screenTexCoords.data(),
      GL_STATIC_DRAW
  );
  vboDeleted = false;
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

} // namespace wgc0310
