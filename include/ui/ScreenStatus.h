#ifndef PROJECT_WG_SCREEN_STATUS_H
#define PROJECT_WG_SCREEN_STATUS_H

#include <cstdint>

#include "cwglx/GL.h"
#include "util/Derive.h"
#include "Animation.h"

namespace cw {

class Texture2D;

} // namespace cw

class ScreenStatus final {
public:
  ScreenStatus();

  void PlayStaticAnimation(StaticScreen *staticScreen);

  void PlayAnimation(Animation *animation);

  void DrawOnScreen(GLFunctions *f);

  void NextFrame();
  void Reset();

  CW_DERIVE_UNCOPYABLE(ScreenStatus)
  CW_DERIVE_UNMOVABLE(ScreenStatus)

private:
  bool m_IsPlayingStaticAnimation;
  bool m_IsPlayingDynamicAnimation;

  StaticScreen *m_StaticScreen;
  Animation *m_Animation;
  std::uint64_t m_Frame;
};

#endif // PROJECT_WG_SCREEN_STATUS_H
