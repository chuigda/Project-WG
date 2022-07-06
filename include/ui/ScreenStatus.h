#ifndef PROJECT_WG_SCREEN_STATUS_H
#define PROJECT_WG_SCREEN_STATUS_H

#include <QListWidgetItem>

#include "cwglx/GL.h"
#include "util/Derive.h"
#include "Animation.h"

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

  [[nodiscard]] constexpr inline cw::Texture2D* GetTexture() const noexcept {
    return m_Texture2D;
  }

  CW_DERIVE_UNCOPYABLE(StaticScreenItem)
  CW_DERIVE_UNMOVABLE(StaticScreenItem)

private:
  cw::Texture2D *m_Texture2D;
};

class AnimationItem final : public QListWidgetItem {
public:
  AnimationItem(const QString &text, Animation *animation)
    : QListWidgetItem(text),
      m_Animation(animation)
  {}

  [[nodiscard]] constexpr inline Animation *GetAnimation() const noexcept {
    return m_Animation;
  }

  CW_DERIVE_UNCOPYABLE(AnimationItem)
  CW_DERIVE_UNMOVABLE(AnimationItem)

private:
  Animation *m_Animation;
};

class ScreenStatus final : public QObject {
  Q_OBJECT

public:
  ScreenStatus();

  void PlayStaticAnimation(StaticScreenItem *staticScreenItem);

  void PlayAnimation(AnimationItem *animationItem);

  void DrawOnScreen(GLFunctions *f);

  CW_DERIVE_UNCOPYABLE(ScreenStatus)
  CW_DERIVE_UNMOVABLE(ScreenStatus)

  void Reset();

private slots:
  void NextFrame();

private:
  bool m_IsPlayingStaticAnimation;
  bool m_IsPlayingDynamicAnimation;

  StaticScreenItem *m_StaticScreenItem;
  AnimationItem *m_AnimationItem;
  QTimer *m_Timer;
  std::uint64_t m_Frame;
};

#endif // PROJECT_WG_SCREEN_STATUS_H
