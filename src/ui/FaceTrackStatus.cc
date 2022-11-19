#include "ui/FaceTrackStatus.h"

FaceTrackStatus::FaceTrackStatus()
  : m_EyeStatus(EyeStatus::Open),
    m_EyeStatusDuration(1000),
    m_CurrentPose {
      .rotationX = 0.0,
      .rotationY = 0.0,
      .rotationZ = 0.0,
      .mouthStatus = cw::HeadPose::Close
    }
{}

void FaceTrackStatus::DrawOnScreen(GLFunctions *f) {

}
