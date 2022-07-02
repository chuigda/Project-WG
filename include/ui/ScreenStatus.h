#ifndef PROJECT_WG_SCREEN_STATUS_H
#define PROJECT_WG_SCREEN_STATUS_H

#include <QListWidgetItem>

#include "cwglx/GL.h"
#include "util/Derive.h"

namespace cw {

class Texture2D;

} // namespace cw

class StaticScreenItem final : public QListWidgetItem {
public:
  StaticScreenItem(const QString &text, cw::Texture2D *texture2D)
      : QListWidgetItem(text),
        m_Texture2D(texture2D)
  {}
  ~StaticScreenItem() final = default;

  [[nodiscard]] constexpr inline cw::Texture2D*
  GetTexture() const noexcept {
    return m_Texture2D;
  }

private:
  cw::Texture2D *m_Texture2D;
};

class ScreenStatus final {
public:
  ScreenStatus();

  void PlayStaticAnimation(StaticScreenItem *staticScreenItem);

  void DrawOnScreen(GLFunctions *f);

  CW_DERIVE_UNCOPYABLE(ScreenStatus);
  CW_DERIVE_UNMOVABLE(ScreenStatus);

  void Reset();

private:
  bool m_IsPlayingStaticAnimation;
  bool m_IsPlayingDynamicAnimation;

  StaticScreenItem *m_StaticScreenItem;
};

#endif // PROJECT_WG_SCREEN_STATUS_H
