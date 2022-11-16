#include "wgc0310/BodyStatus.h"
#include <QDebug>

namespace wgc0310 {

PlayAnimationStatus::PlayAnimationStatus() noexcept
  : m_Animation(nullptr),
    m_CurrentSection(0),
    m_CurrentFrameCount(0)
{}

void
PlayAnimationStatus::SetAnimation(BodyAnimation const* animation) noexcept {
  m_Animation = animation;
  m_CurrentSection = 0;
  m_CurrentFrameCount = 0;
}

bool PlayAnimationStatus::NextFrame(BodyStatus *bodyStatus) noexcept {
  if (!m_Animation) {
    return false;
  }

restart:
  if (m_CurrentSection >= m_Animation->size()) {
    return false;
  }

  AnimationSection const& section = (*m_Animation)[m_CurrentSection];
  if (m_CurrentFrameCount >= section.totalFrameCount) {
    m_CurrentSection += 1;
    m_CurrentFrameCount = 0;
    goto restart;
  }

  section.ApplyAnimationFrame(bodyStatus, m_CurrentFrameCount);
  m_CurrentFrameCount += 1;
  return true;
}

} // namespace wgc0310
