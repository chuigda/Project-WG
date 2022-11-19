#include "ui/FaceTrackStatus.h"

#include <cstdlib>
#include <ctime>
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
    m_MouthStatusFrame(0)
{
  std::srand(std::time(nullptr));
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
        break;
    }
  }

  m_MouthStatusFrame += 1;
}

void FaceTrackStatus::DrawOnScreen(GLFunctions *f) {

}
