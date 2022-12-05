#ifndef PROJECT_WG_UINEXT_CONTROL_PANEL_H
#define PROJECT_WG_UINEXT_CONTROL_PANEL_H

#include <QWidget>
#include <QThread>
#include "wgc0310/ScreenAnimationStatus.h"
#include "wgc0310/HeadStatus.h"
#include "wgc0310/BodyStatus.h"
#include "ui/CameraEntityStatus.h"
#include "ui_next/CloseSignallingWidget.h"
#include "BodyControl.h"

class QPushButton;
class GLWindow;
class CameraControl;
class TrackControl;
class ScreenAnimationControl;

class ControlPanel final : public CloseSignallingWidget {
  Q_OBJECT

public:
  ControlPanel();
  ~ControlPanel() noexcept final;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void DoneBodyAnimation();
#pragma clang diagnostic pop

private:
  // Worker thread, must be initialized very first
  QThread m_WorkerThread;

  // status
  CameraEntityStatus m_CameraEntityStatus;
  wgc0310::HeadStatus m_HeadStatus;
  wgc0310::ScreenAnimationStatus m_ScreenAnimationStatus;
  wgc0310::ScreenDisplayMode m_ScreenDisplayMode;
  wgc0310::BodyStatus m_BodyStatus;

  // widgets
  GLWindow *m_GLWindow;
  CameraControl *m_CameraControl;
  TrackControl *m_TrackControl;
  ScreenAnimationControl *m_ScreenAnimationControl;
  BodyControl *m_BodyControl;

  QPushButton *m_OpenGLSettingsButton;
  QPushButton *m_CameraSettingsButton;
  QPushButton *m_BodyAnimationButton;
  QPushButton *m_FaceAnimationButton;
  QPushButton *m_PoseEstimationButton;
  QPushButton *m_AboutButton;
};

#endif // PROJECT_WG_UINEXT_CONTROL_PANEL_H
