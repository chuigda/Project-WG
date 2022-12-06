#include "include/wgc0310/ScreenAnimationStatus.h"

#include <QTimer>
#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"
#include "util/DynLoad.h"

namespace wgc0310 {

ScreenAnimation::ScreenAnimation(const WGAPI_Animation *rawAnimation,
                                 void *rawHandle)
  : rawAnimation(rawAnimation),
    m_RawHandle(rawHandle),
    m_Context(nullptr) {}

ScreenAnimation::~ScreenAnimation() {
  if (m_Context) {
    qWarning() << "Called dtor of animation"
               << rawAnimation->name
               << "before deleting OpenGL resources";
  }

  cw::DetachSharedObject(m_RawHandle);
}

bool ScreenAnimation::Initialize(GLFunctions *f) {
  m_Context = rawAnimation->initContextFn(f);
  return static_cast<bool>(m_Context);
}

bool ScreenAnimation::Rewind(GLFunctions *f) {
  return rawAnimation->rewindContextFn(m_Context, f);
}

bool ScreenAnimation::PlayFrame(GLFunctions *f, std::uint64_t frame) {
  return rawAnimation->playAnimationFrameFn(m_Context, f, frame);
}

WGAPI_Error ScreenAnimation::GetError() {
  return rawAnimation->getErrorFn(m_Context);
}

void ScreenAnimation::Delete(GLFunctions *f) {
  rawAnimation->destroyContextFn(m_Context, f);
}

ScreenAnimationStatus::ScreenAnimationStatus()
  : m_IsPlayingStaticAnimation(false),
    m_IsPlayingDynamicAnimation(false),
    m_NeedRewind(false),
    m_StaticScreen(nullptr),
    m_Animation(nullptr),
    m_Frame(0) {}

void ScreenAnimationStatus::PlayStaticAnimation(StaticScreenImage *staticScreen) {
  Reset();

  m_IsPlayingStaticAnimation = true;
  m_StaticScreen = staticScreen;
}

void ScreenAnimationStatus::PlayAnimation(ScreenAnimation *animation) {
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
      if (!m_Animation->Rewind(f)) {
        qWarning() << "ScreenAnimationStatus::DrawOnScreen:"
                   << "error rewinding animation"
                   << m_Animation->rawAnimation->name;
      }
    }

    f->glScalef(1.0f, -1.0f, 1.0f);
    f->glFrontFace(GL_CW);

    bool playResult = m_Animation->PlayFrame(f, m_Frame);
    if (!playResult) {
      qWarning() << "ScreenAnimationStatus::DrawOnScreen:"
                 << "error playing the"
                 << m_Frame
                 << "th frame of animation"
                 << m_Animation->rawAnimation->name;
    }
  }
}

void ScreenAnimationStatus::Reset() {
  m_IsPlayingStaticAnimation = false;
  m_IsPlayingDynamicAnimation = false;
  m_StaticScreen = nullptr;
  m_Frame = 0;
}

void ScreenAnimationStatus::NextFrame() {
  if (m_IsPlayingDynamicAnimation) {
    m_Frame++;
  }
}

} // namespace wgc0310
