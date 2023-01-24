#ifndef PROJECT_WG_BODY_STATUS_H
#define PROJECT_WG_BODY_STATUS_H

#include <vector>
#include <QtGui/qopengl.h>
#include <glm/vec4.hpp>
#include "wgc0310/BodyAnim.h"

namespace wgc0310 {

class BodyStatus;

class PlayAnimationStatus {
public:
  explicit PlayAnimationStatus() noexcept;

  void SetAnimation(BodyAnimation const* animation) noexcept;

  [[nodiscard]] constexpr inline bool IsPlayingAnimation() const noexcept {
    return static_cast<bool>(m_Animation);
  }

  bool NextTick(BodyStatus* bodyStatus) noexcept;

private:
  BodyAnimation const* m_Animation;
  std::size_t m_CurrentSection;
  std::size_t m_CurrentFrameCount;
};


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

  PlayAnimationStatus playAnimationStatus;

  explicit inline BodyStatus() noexcept
    : colorTimerStatus(Blue),
      blinkFrames(30),
      blinkCounter(0),
      leftArmStatus(),
      rightArmStatus(),
      playAnimationStatus() {}

  inline void Reset() noexcept {
    leftArmStatus.Reset();
    rightArmStatus.Reset();
    playAnimationStatus.SetAnimation(nullptr);
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

  [[nodiscard]] constexpr inline glm::vec4 GetTimerColor() const noexcept {
    switch (colorTimerStatus) {
      case Blue:
        return { 0.0f, 0.75f, 1.0f, 1.0f };
      case Blinking:
        if (blinkCounter > (blinkFrames / 2)) {
          return { 0.4, 0.4f, 0.4f, 1.0f };
        } else {
          return { 1.0f, 0.25f, 0.0f, 1.0f };
        }
      case Gray:
        return { 0.4, 0.4f, 0.4f, 1.0f };
    }
  }
};

} // namespace wgc0310

#endif // PROJECT_WG_BODY_STATUS_H
