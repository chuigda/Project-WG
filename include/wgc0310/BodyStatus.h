#ifndef PROJECT_WG_BODY_STATUS_H
#define PROJECT_WG_BODY_STATUS_H

#include <QtGui/qopengl.h>
#include "cwglx/Color.h"

namespace wgc0310 {

class ArmStatus {
public:
  GLfloat rotation[5] = {0.0f, -90.0f, 0.0f, 0.0f, 0.0f};

  constexpr inline void Reset() noexcept {
    rotation[0] = 0.0f;
    rotation[1] = -90.0f;
    rotation[2] = 0.0f;
    rotation[3] = 0.0f;
    rotation[4] = 0.0f;
  }
};

class BodyStatus {
public:
  enum ColorTimerStatus {
    Blue, Blinking, Gray
  };

  ColorTimerStatus colorTimerStatus;
  unsigned blinkFrames;
  unsigned blinkCounter;
  ArmStatus leftArmStatus;
  ArmStatus rightArmStatus;

  explicit constexpr inline BodyStatus() noexcept
    : colorTimerStatus(Blue),
      blinkFrames(30),
      blinkCounter(0),
      leftArmStatus(),
      rightArmStatus() {}

  constexpr inline void Reset() noexcept {
    colorTimerStatus = Blue;
    blinkFrames = 30;
    blinkCounter = 0;
    leftArmStatus.Reset();
    rightArmStatus.Reset();
  }

  constexpr inline void StartBlinking() noexcept {
    colorTimerStatus = Blinking;
    blinkCounter = 0;
  }

  constexpr inline void SetBlinkFrames(unsigned frames) noexcept {
    blinkFrames = frames;
    if (blinkCounter >= blinkFrames) {
      blinkCounter = 0;
    }
  }

  constexpr inline void NextTick() noexcept {
    if (colorTimerStatus == Blinking) {
      blinkCounter += 1;
      if (blinkCounter >= blinkFrames) {
        blinkCounter = 0;
      }
    }
  }

  [[nodiscard]] constexpr inline cw::RGBAColorF GetTimerColor() const noexcept {
    switch (colorTimerStatus) {
      case Blue:
        return cw::RGBAColorF{0.0f, 0.75f, 1.0f, 1.0f};
      case Blinking:
        if (blinkCounter > (blinkFrames / 2)) {
          return cw::RGBAColorF{0.4, 0.4f, 0.4f, 1.0f};
        } else {
          return cw::RGBAColorF{1.0f, 0.25f, 0.0f, 1.0f};
        }
      case Gray:
        return cw::RGBAColorF{0.4, 0.4f, 0.4f, 1.0f};
    }
  }
};

} // namespace wgc0310

#endif // PROJECT_WG_BODY_STATUS_H