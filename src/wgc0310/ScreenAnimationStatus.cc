#include "include/wgc0310/ScreenAnimationStatus.h"

#include <QTimer>
#include <QWidget>
#include "cwglx/GL/GLImpl.h"
#include "cwglx/Base/Texture.h"

namespace wgc0310 {

ScreenAnimationStatus::ScreenAnimationStatus()
  : staticScreen(nullptr),
    animation(nullptr),
    m_NeedRewind(false)
  {}

void ScreenAnimationStatus::PlayStaticAnimation(StaticScreenImage *staticScreen) {
  Reset();
  this->staticScreen = staticScreen;
}

void ScreenAnimationStatus::PlayAnimation(WGAPIAnimation *animation) {
  Reset();

  this->animation = animation;
  m_NeedRewind = true;
}

bool ScreenAnimationStatus::HasThingToDraw() const noexcept {
  return animation || staticScreen;
}

void ScreenAnimationStatus::DrawOnScreen(GLFunctions *f) const noexcept {
  if (staticScreen) {
  } else if (animation) {
    if (m_NeedRewind) {
      m_NeedRewind = false;
      animation->Rewind();
    }

    animation->Draw(f);
  }
}

void ScreenAnimationStatus::Reset() {
  if (animation) {
    QWidget *controlWidget = animation->GetControlWidget();
    if (controlWidget) {
      controlWidget->close();
    }
  }

  staticScreen = nullptr;
  animation = nullptr;
}

void ScreenAnimationStatus::NextTick() {
  if (animation) {
    animation->NextTick();
  }
}

} // namespace wgc0310
