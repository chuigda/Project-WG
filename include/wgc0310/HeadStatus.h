#ifndef PROJECT_WG_WGC0310_HEAD_STATUS_H
#define PROJECT_WG_WGC0310_HEAD_STATUS_H

#include <cstdint>

namespace wgc0310 {

struct HeadStatus {
  float rotationX = 0.0f;
  float rotationY = 0.0f;
  float rotationZ = 0.0f;

  enum class MouthStatus : std::uint8_t {
    Close = 0,
    Open = 1,
    OpenBig = 2
  } mouthStatus = MouthStatus::Close;
};

enum class ScreenDisplayMode : std::uint8_t {
  CapturedExpression,
  SoundWave
};

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_HEAD_STATUS_H
