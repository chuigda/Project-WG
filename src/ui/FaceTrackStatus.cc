#include "ui/FaceTrackStatus.h"

#include <cstdlib>
#include <ctime>
#include <QImage>
#include "cwglx/GLImpl.h"

FaceTrackStatus::FaceTrackStatus()
  : pose {
      .rotationX = 0.0,
      .rotationY = 0.0,
      .rotationZ = 0.0,
      .mouthStatus = HeadPose::Close,
    },
    m_EyeStatus(EyeStatus::Open),
    m_EyeStatusFrame(0),
    m_EyeStatusDuration(400),
    m_EyeTexture(nullptr),
    m_MouthTexture(nullptr),
    m_VBO { 0, 0 }
{
  m_VolumeLevels.resize(160, 0.05f);

  m_VolumeVertices.reserve(640);
  for (GLuint i = 0; i < 160; i++) {
    float x = static_cast<float>(i) * 4.0f - 320.0f;
    m_VolumeVertices.emplace_back(x, 0.1f);
    m_VolumeVertices.emplace_back(x, -0.1f);
    m_VolumeVertices.emplace_back(x + 4.0f, -0.1f);
    m_VolumeVertices.emplace_back(x + 4.0f, 0.1f);
  }

  m_VolumeIndices.reserve(640);
  for (GLuint i = 0; i < 640; i++) {
      m_VolumeIndices.push_back(i);
  }

  std::srand(std::time(nullptr));
}

void FaceTrackStatus::Initialize(GLFunctions *f) {
  QImage eye9Image(":/eye.9.bmp");
  QImage halfFaceImage(":/half-face.bmp");
  assert(!eye9Image.isNull());
  assert(!halfFaceImage.isNull());

  m_EyeTexture = std::make_unique<cw::Texture2D>(eye9Image, f);
  m_MouthTexture = std::make_unique<cw::Texture2D>(halfFaceImage, f);

  f->glGenBuffers(2, m_VBO.data());

  f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
  f->glBufferData(GL_ARRAY_BUFFER,
                  static_cast<GLsizei>(m_VolumeVertices.size() * sizeof(cw::Vertex2DF)),
                  m_VolumeVertices.data(),
                  GL_STREAM_DRAW);

  f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[1]);
  f->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                  static_cast<GLsizei>(m_VolumeIndices.size() * sizeof(GLuint)),
                  m_VolumeIndices.data(),
                  GL_STATIC_DRAW);
}

void FaceTrackStatus::NextFrame() {
  m_EyeStatusFrame += 1;
  if (m_EyeStatusFrame > m_EyeStatusDuration) {
    m_EyeStatusFrame = 0;
    m_EyeStatus = static_cast<EyeStatus>(-m_EyeStatus);
    switch (m_EyeStatus) {
      case Open:
        m_EyeStatusDuration = std::rand() % 200 + 500;
        break;
      case Blinking:
        m_EyeStatusDuration = 25;
        break;
    }
  }
}

static void DrawEye(GLFunctions *f,
                    float top,
                    float bottom,
                    float left,
                    float right)
{
  f->glBegin(GL_QUADS);
  {
    f->glTexCoord2f(0.0f, 0.0f);
    f->glVertex2f(left, top);

    f->glTexCoord2f(0.0f, 0.25f);
    f->glVertex2f(left, top - 7.0f);

    f->glTexCoord2f(1.0f, 0.25f);
    f->glVertex2f(right, top - 7.0f);

    f->glTexCoord2f(1.0f, 0.0f);
    f->glVertex2f(right, top);
  }

  {
    f->glTexCoord2f(0.0f, 0.25f);
    f->glVertex2f(left, top - 7.0f);

    f->glTexCoord2f(0.0f, 0.75f);
    f->glVertex2f(left, bottom + 7.0f);

    f->glTexCoord2f(1.0f, 0.75f);
    f->glVertex2f(right, bottom + 7.0f);

    f->glTexCoord2f(1.0f, 0.25f);
    f->glVertex2f(right, top - 7.0f);
  }

  {
    f->glTexCoord2f(0.0f, 0.75f);
    f->glVertex2f(left, bottom + 7.0f);

    f->glTexCoord2f(0.0f, 1.0f);
    f->glVertex2f(left, bottom);

    f->glTexCoord2f(1.0f, 1.0f);
    f->glVertex2f(right, bottom);

    f->glTexCoord2f(1.0f, 0.75f);
    f->glVertex2f(right, bottom + 7.0f);
  }
  f->glEnd();
}

void FaceTrackStatus::DrawOnScreen(GLFunctions *f) {
  f->glScalef(1.0f, -1.0f, 1.0f);
  f->glFrontFace(GL_CW);
  if (pose.screenControlStatus == HeadPose::Soundwave) {
    RecomputeVertices();

    f->glPushMatrix();
    f->glColor4f(0.5f, 1.0f, 1.0f, 1.0f);
    f->glBufferData(GL_ARRAY_BUFFER,
                    static_cast<GLsizei>(m_VolumeVertices.size() * sizeof(cw::Vertex2DF)),
                    m_VolumeVertices.data(),
                    GL_STREAM_DRAW);

    f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
    {
      f->glEnableClientState(GL_VERTEX_ARRAY);
      f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
      f->glVertexPointer(2, GL_FLOAT, 0, nullptr);

      f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[1]);
      f->glDrawElements(GL_QUADS,
                        static_cast<GLsizei>(m_VolumeIndices.size()),
                        GL_UNSIGNED_INT,
                        nullptr);
    }
    f->glPopClientAttrib();
    f->glPopMatrix();
  } else {
    f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    m_MouthTexture->BeginTexture(f);

    f->glBegin(GL_QUADS);
    f->glTexCoord2f(0.0f, 0.0f);
    f->glVertex2f(-320.0f, 0.0f);

    f->glTexCoord2f(0.0f, 1.0f);
    f->glVertex2f(-320.0f, -240.0f);

    f->glTexCoord2f(1.0f, 1.0f);
    f->glVertex2f(320.0f, -240.0f);

    f->glTexCoord2f(1.0f, 0.0f);
    f->glVertex2f(320.0f, 0.0f);
    f->glEnd();

    float eyeTop;
    float eyeBottom;

    if (m_EyeStatus == EyeStatus::Blinking) {
      float ratio = static_cast<float>(m_EyeStatusFrame)
                    / static_cast<float>(m_EyeStatusDuration);
      float len;
      if (ratio > 0.5) {
        float r = (ratio - 0.5f) * 2.0f;
        r = 0.4f * r * r * r - 0.6f * r * r + 1.2f * r;
        len = 4.0f + 48.0f * r;
      } else {
        float r = (0.5f - ratio) * 2.0f;
        r = 0.4f * r * r * r - 0.6f * r * r + 1.2f * r;
        len = 4.0f + 48.0f * r;
      }
      eyeTop = 42.0f + len;
      eyeBottom = 42.0f - len;
    } else {
      eyeTop = 42.0f + 52.0f;
      eyeBottom = 42.0f - 52.0f;
    }

    f->glTranslatef(0.0f, 0.0f, 0.1f);

    f->glPushMatrix();
    f->glScalef(1.0f / 0.75f, 1.0f / 0.75f, 1.0f);
    m_EyeTexture->BeginTexture(f);

    DrawEye(f, eyeTop, eyeBottom, -118.0f, -90.0f);
    DrawEye(f, eyeTop, eyeBottom, 90.0f, 118.0f);

    f->glPopMatrix();
  }
}

void FaceTrackStatus::PushVolumeSample(qreal sample) {
  m_VolumeLevels.pop_front();
  m_VolumeLevels.push_back(sample);
}

void FaceTrackStatus::Destroy(GLFunctions *f) {
  m_EyeTexture->DeleteTexture(f);
  m_MouthTexture->DeleteTexture(f);
}

void FaceTrackStatus::RecomputeVertices() {
  for (size_t i = 0; i < 160; i++) {
    m_VolumeVertices[i * 4].SetY(static_cast<GLfloat>(m_VolumeLevels[i]) * 200.0f);
    m_VolumeVertices[i * 4 + 1].SetY(static_cast<GLfloat>(m_VolumeLevels[i]) * -200.0f);
    m_VolumeVertices[i * 4 + 2].SetY(static_cast<GLfloat>(m_VolumeLevels[i]) * -200.0f);
    m_VolumeVertices[i * 4 + 3].SetY(static_cast<GLfloat>(m_VolumeLevels[i]) * 200.0f);
  }
}
