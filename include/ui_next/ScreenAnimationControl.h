#ifndef SCREENANIMATIONCONTROL_H
#define SCREENANIMATIONCONTROL_H

#include <memory>
#include <QWidget>
#include "wgc0310/ScreenAnimationStatus.h"

class GLWindow;

class ScreenAnimationControl : public QWidget {
  Q_OBJECT

public:
  ScreenAnimationControl(GLWindow *glWindow,
                         wgc0310::ScreenAnimationStatus *animationStatus);

public slots:
  void GLContextReady();

private:
  void ReloadStaticImages();
  void ReloadScreenAnimations();

private:
  GLWindow *m_GLWindow;

  wgc0310::ScreenAnimationStatus *m_ScreenAnimationStatus;
  std::vector<std::unique_ptr<wgc0310::StaticScreenImage>> m_StaticImages;
  std::vector<std::unique_ptr<wgc0310::ScreenAnimation>> m_ScreenAnimations;
};

#endif // SCREENANIMATIONCONTROL_H
