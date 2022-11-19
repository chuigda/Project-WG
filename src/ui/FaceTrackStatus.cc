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
    m_MouthTexture(nullptr)
{
  std::srand(std::time(nullptr));
}

void FaceTrackStatus::Initialize(GLFunctions *f) {
  QImage eye9Image(":/eye.9.bmp");
  QImage halfFaceImage(":/half-face.bmp");
  QImage halfFaceMouthOpenImage(":/half-face-mouth-open.bmp");

  assert(!eye9Image.isNull());
  assert(!halfFaceImage.isNull());

  m_EyeTexture = std::make_unique<cw::Texture2D>(eye9Image, f);
  m_MouthTexture = std::make_unique<cw::Texture2D>(halfFaceImage, f);
  m_MouthOpenTexture = std::make_unique<cw::Texture2D>(halfFaceMouthOpenImage, f);
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
  f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  if (pose.mouthStatus == HeadPose::Open) {
    m_MouthOpenTexture->BeginTexture(f);
  } else {
    m_MouthTexture->BeginTexture(f);
  }

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

void FaceTrackStatus::Destroy(GLFunctions *f) {
  m_EyeTexture->DeleteTexture(f);
  m_MouthTexture->DeleteTexture(f);
  m_MouthOpenTexture->DeleteTexture(f);
}
