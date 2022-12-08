#include "include/wgc0310/ScreenAnimationStatus.h"

#include <QTimer>
#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"
#include "util/DynLoad.h"

namespace wgc0310 {

ScreenAnimationStatus::ScreenAnimationStatus()
  : m_IsPlayingStaticAnimation(false),
    m_IsPlayingDynamicAnimation(false),
    m_NeedRewind(false),
    m_StaticScreen(nullptr),
    m_Animation(nullptr)
  {}

void ScreenAnimationStatus::PlayStaticAnimation(StaticScreenImage *staticScreen) {
  Reset();

  m_IsPlayingStaticAnimation = true;
  m_StaticScreen = staticScreen;
}

void ScreenAnimationStatus::PlayAnimation(WGAPIAnimation *animation) {
  Reset();

  m_IsPlayingDynamicAnimation = true;
  m_Animation = animation;
  m_NeedRewind = true;
}

bool ScreenAnimationStatus::HasThingToDraw() const noexcept {
  return m_IsPlayingStaticAnimation || m_IsPlayingDynamicAnimation;
}

void ScreenAnimationStatus::DrawOnScreen(GLFunctions *f) const noexcept {
  if (m_IsPlayingStaticAnimation) {
    f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    f->glTranslatef(0.0f, 0.0f, 0.1f);

    m_StaticScreen->texture->BeginTexture(f);
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
  } else if (m_IsPlayingDynamicAnimation) {
    if (m_NeedRewind) {
      m_NeedRewind = false;
      m_Animation->Rewind();
    }

    f->glScalef(1.0f, -1.0f, 1.0f);
    f->glFrontFace(GL_CW);

    m_Animation->Draw(f);
  }
}

void ScreenAnimationStatus::Reset() {
  m_IsPlayingStaticAnimation = false;
  m_IsPlayingDynamicAnimation = false;
  m_StaticScreen = nullptr;
}

void ScreenAnimationStatus::NextTick() {
  if (m_IsPlayingDynamicAnimation) {
    m_Animation->NextTick();
  }
}

} // namespace wgc0310
