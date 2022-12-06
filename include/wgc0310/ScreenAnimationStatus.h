#ifndef PROJECT_WG_SCREEN_STATUS_H
#define PROJECT_WG_SCREEN_STATUS_H

#include <cstdint>
#include <QString>

#include "cwglx/GL.h"
#include "util/Derive.h"
#include "wgc0310/ScreenAnimation.h"

namespace cw {
class Texture2D;
} // namespace cw

namespace wgc0310 {

struct StaticScreenImage final {
  QString imageName;
  std::unique_ptr<cw::Texture2D> texture;
};

class ScreenAnimation final {
public:
  explicit ScreenAnimation(WGAPI_Animation const* rawAnimation,
                           void *rawHandle);
  ~ScreenAnimation();

  CW_DERIVE_UNCOPYABLE(ScreenAnimation)
  CW_DERIVE_UNMOVABLE(ScreenAnimation)

  [[nodiscard]] bool Initialize(GLFunctions *f);
  [[nodiscard]] bool Rewind(GLFunctions *f);
  [[nodiscard]] bool PlayFrame(GLFunctions *f, std::uint64_t frame);
  [[nodiscard]] WGAPI_Error GetError();
  void Delete(GLFunctions *f);

  WGAPI_Animation const* rawAnimation;

private:
  void *m_RawHandle;
  void *m_Context;
};

class ScreenAnimationStatus final {
public:
  ScreenAnimationStatus();

  void PlayStaticAnimation(StaticScreenImage *staticScreen);

  void PlayAnimation(ScreenAnimation *animation);

  [[nodiscard]] bool HasThingToDraw() const noexcept;

  void DrawOnScreen(GLFunctions *f) const noexcept;

  void NextFrame();
  void Reset();

  CW_DERIVE_UNCOPYABLE(ScreenAnimationStatus)
  CW_DERIVE_UNMOVABLE(ScreenAnimationStatus)

private:
  bool m_IsPlayingStaticAnimation;
  bool m_IsPlayingDynamicAnimation;
  mutable bool m_NeedRewind;

  StaticScreenImage *m_StaticScreen;
  ScreenAnimation *m_Animation;
  std::uint64_t m_Frame;
};

} // namespace wgc0310

#endif // PROJECT_WG_SCREEN_STATUS_H
