#ifndef PROJECT_WG_WGC0310_BONE_ANIM_H
#define PROJECT_WG_WGC0310_BONE_ANIM_H

#include <cstdint>
#include <vector>
#include <memory>
#include <QtGui/qopengl.h>
#include "wgc0310/BodyStatus.h"

namespace wgc0310 {

struct AnimationCommand {
  bool isLeft;
  std::size_t rotationAxisIndex;
  GLfloat frameRotation;
  std::size_t frameCount;
};

struct AnimationSection {
  std::size_t totalFrameCount;
  std::vector<AnimationCommand> commandList;

  AnimationSection() noexcept;

  AnimationSection(AnimationSection const&) = delete;
  AnimationSection& operator=(AnimationSection const&) = delete;

  AnimationSection(AnimationSection&& other) = default;
  AnimationSection& operator=(AnimationSection&& other) = default;

  void AddCommand(AnimationCommand command) noexcept;

  void ApplyAnimationFrame(BodyStatus *bodyStatus,
                           std::size_t frame) const noexcept;
};

using BodyAnimation = std::vector<AnimationSection>;

class PlayAnimationStatus {
public:
  explicit PlayAnimationStatus(BodyAnimation const* animation) noexcept;

  bool NextFrame(BodyStatus* bodyStatus) noexcept;

private:
  BodyAnimation const* m_Animation;
  std::size_t m_CurrentSection;
  std::size_t m_CurrentFrameCount;
};

std::unique_ptr<BodyAnimation> LoadBodyAnimation(const char *fileName);

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_BONE_ANIM_H
