#ifndef PROJECT_WG_SCREEN_STATUS_H
#define PROJECT_WG_SCREEN_STATUS_H

#include <cstdint>
#include <QString>

#include "cwglx/GL.h"
#include "util/Derive.h"
#include "include/wgc0310/api/ScreenAnimation.h"

namespace cw {
class Texture2D;
} // namespace cw

namespace wgc0310 {

struct StaticScreenImage final {
  QString imageName;
  std::unique_ptr<cw::Texture2D> texture;
};

class ScreenAnimationStatus final {
public:
  ScreenAnimationStatus();

  void PlayStaticAnimation(StaticScreenImage *staticScreen);

  void PlayAnimation(WGAPIAnimation *animation);

  [[nodiscard]] bool HasThingToDraw() const noexcept;

  void DrawOnScreen(GLFunctions *f) const noexcept;

  void NextTick();
  void Reset();

  CW_DERIVE_UNCOPYABLE(ScreenAnimationStatus)
  CW_DERIVE_UNMOVABLE(ScreenAnimationStatus)

private:
  bool m_IsPlayingStaticAnimation;
  bool m_IsPlayingDynamicAnimation;
  mutable bool m_NeedRewind;

  StaticScreenImage *m_StaticScreen;
  WGAPIAnimation *m_Animation;
};

} // namespace wgc0310

#endif // PROJECT_WG_SCREEN_STATUS_H
