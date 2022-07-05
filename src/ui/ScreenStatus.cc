#include "ui/ScreenStatus.h"

#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"

ScreenStatus::ScreenStatus()
  : m_IsPlayingStaticAnimation(false),
    m_IsPlayingDynamicAnimation(false),
    m_StaticScreenItem(nullptr),
    m_AnimationItem(nullptr),
    m_Frame(0)
{}

void ScreenStatus::PlayStaticAnimation(StaticScreenItem *staticScreenItem) {
  Reset();

  m_IsPlayingStaticAnimation = true;
  m_StaticScreenItem = staticScreenItem;
}

void ScreenStatus::PlayAnimation(AnimationItem *animationItem) {
  Reset();

  m_IsPlayingDynamicAnimation = true;
  m_AnimationItem = animationItem;
}

void ScreenStatus::DrawOnScreen(GLFunctions *f) {
  if (m_IsPlayingStaticAnimation && m_StaticScreenItem) {
    f->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    f->glTranslatef(0.0f, 0.0f, 0.1f);

    m_StaticScreenItem->GetTexture()->BeginTexture(f);
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
  } else if (m_IsPlayingDynamicAnimation && m_AnimationItem) {
    Animation *animation = m_AnimationItem->GetAnimation();

    bool playResult = animation->PlayAnimationFrame(f, m_Frame);
    if (!playResult) {
      qWarning() << "ScreenStatus::DrawOnScreen:"
                 << "error playing the"
                 << m_Frame
                 << "th frame of animation"
                 << animation->GetAnimationName()
                 << "(from file \"" + animation->GetFileName() + "\"";
      m_Frame += 1;
    }
  }
}

void ScreenStatus::Reset() {
  m_IsPlayingStaticAnimation = false;
  m_IsPlayingDynamicAnimation = false;
  m_StaticScreenItem = nullptr;
  m_Frame = 0;
}
