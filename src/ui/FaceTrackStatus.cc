#include "ui/FaceTrackStatus.h"

#include <cstdlib>
#include <ctime>
#include <QImage>
#include "cwglx/GLImpl.h"

FaceTrackStatus::FaceTrackStatus()
  : m_EyeStatus(EyeStatus::Open),
    m_EyeStatusFrame(0),
    m_EyeStatusDuration(1000),
    m_CurrentPose {
      .rotationX = 0.0,
      .rotationY = 0.0,
      .rotationZ = 0.0,
      .mouthStatus = cw::HeadPose::Close,
    },
    m_MouthStatusFrame(0),
    m_EyeTexture(nullptr),
    m_MouthTexture(nullptr)
{
  std::srand(std::time(nullptr));
}

void FaceTrackStatus::Initialize(GLFunctions *f) {
  QImage eye9Image(":/eye.9.bmp");
  QImage halfFaceImage(":/half-face.bmp");

  assert(!eye9Image.isNull());
  assert(!halfFaceImage.isNull());

  m_EyeTexture = std::make_unique<cw::Texture2D>(eye9Image, f);
  m_MouthTexture = std::make_unique<cw::Texture2D>(halfFaceImage, f);
}


void FaceTrackStatus::FeedHeadPose(cw::HeadPose pose) {
  if (m_MouthStatusFrame > config.mouthDuration
      && pose.mouthStatus != m_CurrentPose.mouthStatus) {
    m_CurrentPose.mouthStatus = pose.mouthStatus;
    m_MouthStatusFrame = 0;
  }

  double oldCoeff = 1 - config.smooth;
  double newCoeff = config.smooth;
  double newX = oldCoeff * m_CurrentPose.rotationX + newCoeff * pose.rotationX;
  double newY = oldCoeff * m_CurrentPose.rotationY + newCoeff * pose.rotationY;
  double newZ = oldCoeff * m_CurrentPose.rotationZ + newCoeff * pose.rotationZ;

  double diffX = newX - m_CurrentPose.rotationX;
  double diffY = newY - m_CurrentPose.rotationY;
  double diffZ = newZ - m_CurrentPose.rotationZ;

  if (std::abs(diffX) > config.deadZoneX) {
    newX = m_CurrentPose.rotationX + std::copysign(diffX, config.deadZoneX);
  }

  if (std::abs(diffY) > config.deadZoneY) {
    newY = m_CurrentPose.rotationY + std::copysign(diffX, config.deadZoneY);
  }

  if (std::abs(diffZ) > config.deadZoneZ) {
    newZ = m_CurrentPose.rotationZ + std::copysign(diffX, config.deadZoneZ);
  }

  m_CurrentPose.rotationX = newX;
  m_CurrentPose.rotationY = newY;
  m_CurrentPose.rotationZ = newZ;
}

void FaceTrackStatus::NextFrame() {
  m_EyeStatusFrame += 1;
  if (m_EyeStatusFrame > m_EyeStatusDuration) {
    m_EyeStatusFrame = 0;
    m_EyeStatus = static_cast<EyeStatus>(-m_EyeStatus);
    switch (m_EyeStatus) {
      case Open:
        m_EyeStatusDuration =
          std::rand() % config.blinkIntervalRange + config.blinkIntervalMin;
        break;
      case Blinking:
        m_EyeStatusDuration = config.blinkDuration;
        break;
    }
  }

  m_MouthStatusFrame += 1;
}

void FaceTrackStatus::DrawOnScreen(GLFunctions *f) {
  float eyeTop;
  float eyeBottom;

  if (m_EyeStatus == EyeStatus::Blinking) {
    float len = 4.0f + 48.0f * (static_cast<float>(m_EyeStatusFrame)
                                / static_cast<float>(m_EyeStatusDuration));
    eyeTop = 42.0f + len;
    eyeBottom = 42.0f - len;
  } else {
    eyeTop = 42.0f + 52.0f;
    eyeBottom = 42.0f - 52.0f;
  }

  f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  f->glTranslatef(0.0f, 0.0f, 0.1f);

  f->glPushMatrix();
  f->glScalef(1.0f / 0.75f, 1.0f / 0.75f, 1.0f);
  m_EyeTexture->BeginTexture(f);
  f->glBegin(GL_QUADS);
  {
    f->glTexCoord2f(0.0f, 0.0f);
    f->glVertex2f(-118.0f, eyeTop);

    f->glTexCoord2f(0.0f, 0.25f);
    f->glVertex2f(-118.0f, eyeTop - 7.0f);

    f->glTexCoord2f(0.0f, 0.75f);
    f->glVertex2f(-118.0f, eyeBottom + 7.0f);

    f->glTexCoord2f(0.0f, 1.0f);
    f->glVertex2f(-118.0f, eyeBottom);

    f->glTexCoord2f(1.0f, 1.0f);
    f->glVertex2f(-90.0f, eyeBottom);

    f->glTexCoord2f(1.0f, 0.75f);
    f->glVertex2f(-90.0f, eyeBottom + 7.0f);

    f->glTexCoord2f(1.0f, 0.25f);
    f->glVertex2f(-90.0f, eyeTop - 7.0f);

    f->glTexCoord2f(1.0f, 0.0f);
    f->glVertex2f(-90.0f, eyeTop);
  }
  f->glEnd();

  f->glBegin(GL_QUADS);
  {
    f->glTexCoord2f(0.0f, 0.0f);
    f->glVertex2f(90.0f, eyeTop);

    f->glTexCoord2f(0.0f, 0.25f);
    f->glVertex2f(90.0f, eyeTop - 7.0f);

    f->glTexCoord2f(0.0f, 0.75f);
    f->glVertex2f(90.0f, eyeBottom + 7.0f);

    f->glTexCoord2f(0.0f, 1.0f);
    f->glVertex2f(90.0f, eyeBottom);

    f->glTexCoord2f(1.0f, 1.0f);
    f->glVertex2f(118.0f, eyeBottom);

    f->glTexCoord2f(1.0f, 0.75f);
    f->glVertex2f(118.0f, eyeBottom + 7.0f);

    f->glTexCoord2f(1.0f, 0.25f);
    f->glVertex2f(118.0f, eyeTop - 7.0f);

    f->glTexCoord2f(1.0f, 0.0f);
    f->glVertex2f(118.0f, eyeTop);
  }
  f->glEnd();

  f->glPopMatrix();
}
