#include "include/wgc0310/ScreenAnimationStatus.h"

#include <QTimer>
#include <QWidget>
#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"

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
    f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    f->glTranslatef(0.0f, 0.0f, 0.1f);

    staticScreen->texture->BeginTexture(f);
    f->glBegin(GL_QUADS);
    {
      f->glTexCoord2f(0.0f, 1.0f);
      f->glVertex2f(-320.0f, 240.0f);

      f->glTexCoord2f(0.0f, 0.0f);
      f->glVertex2f(-320.0f, -240.0f);

      f->glTexCoord2f(1.0f, 0.0f);
      f->glVertex2f(320.0f, -240.0f);

      f->glTexCoord2f(1.0f, 1.0f);
      f->glVertex2f(320.0f, 240.0f);
    }
    f->glEnd();
  } else if (animation) {
    if (m_NeedRewind) {
      m_NeedRewind = false;
      animation->Rewind();
    }

    f->glScalef(1.0f, -1.0f, 1.0f);
    f->glFrontFace(GL_CW);

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
