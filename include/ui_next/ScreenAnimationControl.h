#ifndef PROJECT_WG_UINEXT_SCREEN_ANIMATION_CONTROL_H
#define PROJECT_WG_UINEXT_SCREEN_ANIMATION_CONTROL_H

#include <memory>
#include <QWidget>

#include "wgc0310/HeadStatus.h"
#include "wgc0310/ScreenAnimationStatus.h"
#include "ui_next/CloseSignallingWidget.h"

class GLWindow;
class QHBoxLayout;
class QVBoxLayout;
class QRadioButton;
struct StatusExtra;

class ScreenAnimationControl : public CloseSignallingWidget {
public:
  ScreenAnimationControl(GLWindow *glWindow,
                         wgc0310::ScreenAnimationStatus *animationStatus,
                         wgc0310::ScreenDisplayMode *screenDisplayMode,
                         StatusExtra *statusExtra);

public slots:
  void GLContextReady();

private:
  void ReloadStaticImages();
  void ReloadScreenAnimations();

private:
  GLWindow *m_GLWindow;
  wgc0310::ScreenAnimationStatus *m_ScreenAnimationStatus;
  wgc0310::ScreenDisplayMode *m_ScreenDisplayMode;
  StatusExtra *m_StatusExtra;

  std::vector<wgc0310::StaticScreenImage> m_StaticImages;
  std::vector<std::unique_ptr<wgc0310::WGAPIAnimation>> m_ScreenAnimations;
  std::vector<void*> m_SharedObjects;

  QHBoxLayout *m_StaticImageButtonsLayout;
  QVBoxLayout *m_StaticImageButtonsLayoutV;

  QHBoxLayout *m_ScreenAnimationButtonsLayout;
  QVBoxLayout *m_ScreenAnimationButtonsLayoutV;

  QRadioButton *m_PlayingCapturedExpression;
  QRadioButton *m_PlayingSoundWave;
  QRadioButton *m_PlayingStaticImage;
  QRadioButton *m_PlayingDynamicAnimation;
};

#endif // PROJECT_WG_UINEXT_SCREEN_ANIMATION_CONTROL_H
