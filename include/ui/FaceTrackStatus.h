#ifndef PROJECT_WG_HEAD_POSE_STATUS_H
#define PROJECT_WG_HEAD_POSE_STATUS_H

#include <cstddef>
#include <deque>
#include <memory>

#include "cwglx/GL.h"
#include "cwglx/Texture.h"
#include "util/Derive.h"

struct HeadPose {
  float rotationX;
  float rotationY;
  float rotationZ;

  enum MouthStatus { Close = -1, Open = 1 } mouthStatus = MouthStatus::Close;
};

class FaceTrackStatus {
public:
  FaceTrackStatus();

  HeadPose pose;

  void Initialize(GLFunctions *f);

  void Destroy(GLFunctions *f);

  void NextFrame();

  void DrawOnScreen(GLFunctions *f);

  void PushVolumeSample(qreal sample);

  CW_DERIVE_UNCOPYABLE(FaceTrackStatus)
  CW_DERIVE_UNMOVABLE(FaceTrackStatus)

private:
  void RecomputeVertices();

  enum EyeStatus { Open = 1, Blinking = -1 } m_EyeStatus;
  std::size_t m_EyeStatusFrame;
  std::size_t m_EyeStatusDuration;

  std::unique_ptr<cw::Texture2D> m_EyeTexture;
  std::unique_ptr<cw::Texture2D> m_MouthTexture;

  std::deque<qreal> m_VolumeLevels;
  std::vector<cw::Vertex2DF> m_VolumeVertices;
  std::vector<GLuint> m_VolumeIndices;
  std::array<GLuint, 2> m_VBO;
};

#endif // PROJECT_WG_HEAD_POSE_STATUS_H
