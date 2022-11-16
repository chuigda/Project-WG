#include "wgc0310/BodyStatus.h"

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

  std::vector<AnimationSection> const& sections = m_Animation->GetSections();

restart:
  if (m_CurrentSection >= sections.size()) {
    return false;
  }

  AnimationSection const& section = sections[m_CurrentSection];
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
