#ifndef PROJECT_WG_HEAD_POSE_STATUS_H
#define PROJECT_WG_HEAD_POSE_STATUS_H

#include <cstddef>
#include <memory>

#include "cwglx/GL.h"
#include "cwglx/Texture.h"
#include "util/Derive.h"

struct HeadPose {
  float rotationX;
  float rotationY;
  float rotationZ;

  enum MouthStatus { Close, Open } mouthStatus;
};

class FaceTrackStatus {
public:
  FaceTrackStatus();

  HeadPose pose;

  void Initialize(GLFunctions *f);

  void Destroy(GLFunctions *f);

  void NextFrame();

  void DrawOnScreen(GLFunctions *f);

  CW_DERIVE_UNCOPYABLE(FaceTrackStatus)
  CW_DERIVE_UNMOVABLE(FaceTrackStatus)

private:
  enum EyeStatus { Open = 1, Blinking = -1 } m_EyeStatus;
  std::size_t m_EyeStatusFrame;
  std::size_t m_EyeStatusDuration;

  std::unique_ptr<cw::Texture2D> m_EyeTexture;
  std::unique_ptr<cw::Texture2D> m_MouthTexture;
  std::unique_ptr<cw::Texture2D> m_MouthOpenTexture;
};

#endif // PROJECT_WG_HEAD_POSE_STATUS_H
