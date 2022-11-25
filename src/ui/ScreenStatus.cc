#include "ui/ScreenStatus.h"

#include <QTimer>
#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"
#include "util/DynLoad.h"

AnimationContext::AnimationContext(const WGAPI_Animation *rawAnimation,
                                   void *rawHandle)
  : rawAnimation(rawAnimation),
    m_RawHandle(rawHandle),
    m_Context(nullptr)
{}

AnimationContext::~AnimationContext() {
  if (m_Context) {
    qWarning() << "Called dtor of animation"
               << rawAnimation->name
               << "before deleting OpenGL resources";
  }

  cw::DetachSharedObject(m_RawHandle);
}

bool AnimationContext::Initialize(GLFunctions *f) {
  m_Context = rawAnimation->initContextFn(f);
  return static_cast<bool>(m_Context);
}

bool AnimationContext::Rewind(GLFunctions *f) {
  return rawAnimation->rewindContextFn(m_Context, f);
}

bool AnimationContext::PlayFrame(GLFunctions *f, std::uint64_t frame) {
  return rawAnimation->playAnimationFrameFn(m_Context, f, frame);
}

WGAPI_Error AnimationContext::GetError() {
  return rawAnimation->getErrorFn(m_Context);
}

void AnimationContext::Delete(GLFunctions *f) {
  rawAnimation->destroyContextFn(m_Context, f);
}

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

    bool playResult = m_Animation->PlayFrame(f, m_Frame);
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
