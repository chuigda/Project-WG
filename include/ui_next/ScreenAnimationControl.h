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

class ScreenAnimationControl : public CloseSignallingWidget {
  Q_OBJECT

public:
  ScreenAnimationControl(GLWindow *glWindow,
                         wgc0310::ScreenAnimationStatus *animationStatus,
                         wgc0310::ScreenDisplayMode *screenDisplayMode);

public slots:
  void GLContextReady();

private:
  void ReloadStaticImages();
  void ReloadScreenAnimations();

private:
  GLWindow *m_GLWindow;

  wgc0310::ScreenAnimationStatus *m_ScreenAnimationStatus;
  wgc0310::ScreenDisplayMode *m_ScreenDisplayMode;
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
