#include "ui/FaceTrackStatus.h"

#include <cstdlib>
#include <ctime>
#include <QImage>
#include "cwglx/GLImpl.h"

FaceTrackStatus::FaceTrackStatus()
  : currentPose {
      .rotationX = 0.0,
      .rotationY = 0.0,
      .rotationZ = 0.0,
      .mouthStatus = cw::HeadPose::Close,
    },
    m_EyeStatus(EyeStatus::Open),
    m_EyeStatusFrame(0),
    m_EyeStatusDuration(400),
    m_MouthStatusFrame(0),
    m_LastFeed(currentPose),
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
  m_LastFeed = pose;

  double oldCoeff = 1 - config.smooth;
  double newCoeff = config.smooth;

  double newX = oldCoeff * currentPose.rotationX + newCoeff * pose.rotationX;
  double newY = oldCoeff * currentPose.rotationY + newCoeff * pose.rotationY;
  double newZ = oldCoeff * currentPose.rotationZ + newCoeff * pose.rotationZ;

  double diffX = std::abs(newX - currentPose.rotationX);
  double diffY = std::abs(newY - currentPose.rotationY);
  double diffZ = std::abs(newZ - currentPose.rotationZ);

  if (diffX > 1.5) {
    currentPose.rotationX = newX;
  }

  if (diffY > 1.5) {
    currentPose.rotationY = newY;
  }

  if (diffZ > 1.5) {
    currentPose.rotationZ = newZ;
  }

  if (m_MouthStatusFrame > config.mouthDuration
      && pose.mouthStatus != currentPose.mouthStatus) {
    currentPose.mouthStatus = pose.mouthStatus;
    m_MouthStatusFrame = 0;
  }
}

void FaceTrackStatus::FeedNothing() {
  cw::HeadPose pose = m_LastFeed;

  double oldCoeff = 1 - config.smooth;
  double newCoeff = config.smooth;

  currentPose.rotationX = oldCoeff * currentPose.rotationX + newCoeff * pose.rotationX;
  currentPose.rotationY = oldCoeff * currentPose.rotationY + newCoeff * pose.rotationY;
  currentPose.rotationZ = oldCoeff * currentPose.rotationZ + newCoeff * pose.rotationZ;

  if (m_MouthStatusFrame > config.mouthDuration
      && pose.mouthStatus != currentPose.mouthStatus) {
    currentPose.mouthStatus = pose.mouthStatus;
    m_MouthStatusFrame = 0;
  }
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

  f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  f->glTranslatef(0.0f, 0.0f, 0.1f);

  f->glPushMatrix();
  f->glScalef(1.0f / 0.75f, 1.0f / 0.75f, 1.0f);
  m_EyeTexture->BeginTexture(f);

  DrawEye(f, eyeTop, eyeBottom, -118.0f, -90.0f);
  DrawEye(f, eyeTop, eyeBottom, 90.0f, 118.0f);

  f->glPopMatrix();
}
