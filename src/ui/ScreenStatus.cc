#include "ui/ScreenStatus.h"

#include <QTimer>
#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"

ScreenStatus::ScreenStatus()
  : m_IsPlayingStaticAnimation(false),
    m_IsPlayingDynamicAnimation(false),
    m_StaticScreen(nullptr),
    m_Animation(nullptr),
    m_Frame(0)
{}

void ScreenStatus::PlayStaticAnimation(StaticScreen *staticScreen) {
  Reset();

  m_IsPlayingStaticAnimation = true;
  m_StaticScreen = staticScreen;
}

void ScreenStatus::PlayAnimation(AnimationContext *animation) {
  Reset();

  m_IsPlayingDynamicAnimation = true;
  m_Animation = animation;
}

bool ScreenStatus::HasThingToDraw() const noexcept {
  return m_IsPlayingStaticAnimation || m_IsPlayingDynamicAnimation;
}

void ScreenStatus::DrawOnScreen(GLFunctions *f) {
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
    f->glScalef(1.0f, -1.0f, 1.0f);
    f->glFrontFace(GL_CW);

    bool playResult = m_Animation->PlayAnimationFrame(f, m_Frame);
    if (!playResult) {
      qWarning() << "ScreenStatus::DrawOnScreen:"
                 << "error playing the"
                 << m_Frame
                 << "th frame of animation"
                 << m_Animation->rawAnimation->name;
    }
  }
}

void ScreenStatus::Reset() {
  m_IsPlayingStaticAnimation = false;
  m_IsPlayingDynamicAnimation = false;
  m_StaticScreen = nullptr;
  m_Frame = 0;
}

void ScreenStatus::NextFrame() {
  if (m_IsPlayingDynamicAnimation) {
    m_Frame++;
  }
}
