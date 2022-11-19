#ifndef PROJECT_WG_HEAD_POSE_STATUS_H
#define PROJECT_WG_HEAD_POSE_STATUS_H

#include <cstddef>
#include <memory>

#include "cwglx/GL.h"
#include "cwglx/Texture.h"
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
  std::size_t blinkIntervalRange;
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
      blinkIntervalRange(200),
      blinkDuration(25),
      mouthDuration(15)
  {}
};

class FaceTrackStatus {
public:
  FaceTrackStatus();

  FaceTrackConfig config;

  cw::HeadPose currentPose;

  void Initialize(GLFunctions *f);

  void FeedHeadPose(cw::HeadPose pose);

  void FeedNothing();

  void NextFrame();

  void DrawOnScreen(GLFunctions *f);

  CW_DERIVE_UNCOPYABLE(FaceTrackStatus)
  CW_DERIVE_UNMOVABLE(FaceTrackStatus)

private:
  enum EyeStatus { Open = 1, Blinking = -1 } m_EyeStatus;
  std::size_t m_EyeStatusFrame;
  std::size_t m_EyeStatusDuration;
  std::size_t m_MouthStatusFrame;

  cw::HeadPose m_LastFeed;

  std::unique_ptr<cw::Texture2D> m_EyeTexture;
  std::unique_ptr<cw::Texture2D> m_MouthTexture;
};

#endif // PROJECT_WG_HEAD_POSE_STATUS_H
