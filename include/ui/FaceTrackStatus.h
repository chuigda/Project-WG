#ifndef PROJECT_WG_HEAD_POSE_STATUS_H
#define PROJECT_WG_HEAD_POSE_STATUS_H

#include <cstddef>

#include "cwglx/GL.h"
#include "pose/Pose.h"
#include "util/Derive.h"

struct FaceTrackConfig {
  double smooth;

  double xFix;
  double yFix;
  double zFix;

  double deadZoneX;
  double deadZoneY;
  double deadZoneZ;

  std::size_t blinkIntervalMin;
  std::size_t blinkIntervalMax;
  std::size_t blinkDuration;

  std::size_t mouthDuration;

  constexpr inline FaceTrackConfig() noexcept
    : smooth(0.5),
      xFix(0.0),
      yFix(0.0),
      zFix(0.0),
      deadZoneX(5.0),
      deadZoneY(5.0),
      deadZoneZ(5.0),
      blinkIntervalMin(600),
      blinkIntervalMax(1000),
      blinkDuration(45),
      mouthDuration(25)
  {}
};

class FaceTrackStatus {
public:
  FaceTrackStatus();

  FaceTrackConfig config;

  void FeedHeadPose(cw::HeadPose pose);

  void DrawOnScreen(GLFunctions *f);

  CW_DERIVE_UNCOPYABLE(FaceTrackStatus)
  CW_DERIVE_UNMOVABLE(FaceTrackStatus)

private:
  enum EyeStatus { Open, Blinking } m_EyeStatus;
  std::size_t m_EyeStatusFrame;
  std::size_t m_EyeStatusDuration;

  cw::HeadPose m_CurrentPose;
};

#endif // PROJECT_WG_HEAD_POSE_STATUS_H
