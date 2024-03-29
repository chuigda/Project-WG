#ifndef PROJECT_WG_WGC0310_HEAD_STATUS_H
#define PROJECT_WG_WGC0310_HEAD_STATUS_H

#include <cstdint>

namespace wgc0310 {

struct HeadStatus {
  float rotationX = 0.0f;
  float rotationY = 0.0f;
  float rotationZ = 0.0f;

  float leftEye = 1.0f;
  float rightEye = 1.0f;

  enum class MouthStatus : std::int8_t {
    Close = -1,
    Open = 1
  } mouthStatus = MouthStatus::Close;
};

enum class ScreenDisplayMode : std::int8_t {
  CapturedExpression = -1,
  SoundWave = 1
};

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_HEAD_STATUS_H
