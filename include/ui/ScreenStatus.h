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

struct StaticScreen final {
  QString imageName;
  std::unique_ptr<cw::Texture2D> texture;
};

class AnimationContext final {
public:
  explicit AnimationContext(WGAPI_Animation const* rawAnimation,
                            void *rawHandle);
  ~AnimationContext();

  CW_DERIVE_UNCOPYABLE(AnimationContext)
  CW_DERIVE_UNMOVABLE(AnimationContext)

  [[nodiscard]] bool Initialize(GLFunctions *f);
  [[nodiscard]] bool Rewind(GLFunctions *f);
  [[nodiscard]] bool PlayFrame(GLFunctions *f, std::uint64_t frame);
  [[nodiscard]] WGAPI_Error GetError(void);
  void Delete(GLFunctions *f);

  WGAPI_Animation const* rawAnimation;

private:
  void *m_RawHandle;
  void *m_Context;
};

class ScreenStatus final {
public:
  ScreenStatus();

  void PlayStaticAnimation(StaticScreen *staticScreen);

  void PlayAnimation(AnimationContext *animation);

  [[nodiscard]] bool HasThingToDraw() const noexcept;

  void DrawOnScreen(GLFunctions *f);

  void NextFrame();
  void Reset();

  CW_DERIVE_UNCOPYABLE(ScreenStatus)
  CW_DERIVE_UNMOVABLE(ScreenStatus)

private:
  bool m_IsPlayingStaticAnimation;
  bool m_IsPlayingDynamicAnimation;
  bool m_NeedRewind;

  StaticScreen *m_StaticScreen;
  AnimationContext *m_Animation;
  std::uint64_t m_Frame;
};

#endif // PROJECT_WG_SCREEN_STATUS_H
