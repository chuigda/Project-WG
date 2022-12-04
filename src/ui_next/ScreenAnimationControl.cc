#include "ui_next/ScreenAnimationControl.h"

#include "wgc0310/ScreenAnimationStatus.h"
#include "ui_next/GLWindow.h"

ScreenAnimationControl::ScreenAnimationControl(GLWindow *glWindow,
                                               wgc0310::ScreenAnimationStatus *animationStatus)
  : QWidget(nullptr, Qt::Window),
    m_GLWindow(glWindow),
    m_ScreenAnimationStatus(animationStatus)
{}

void ScreenAnimationControl::GLContextReady() {
  m_GLWindow->RunWithGLContext([this] {
    ReloadStaticImages();
    ReloadScreenAnimations();
  });
}
