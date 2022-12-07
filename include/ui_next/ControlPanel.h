#ifndef PROJECT_WG_UINEXT_CONTROL_PANEL_H
#define PROJECT_WG_UINEXT_CONTROL_PANEL_H

#include <QWidget>
#include <QThread>
#include "wgc0310/ScreenAnimationStatus.h"
#include "wgc0310/HeadStatus.h"
#include "wgc0310/BodyStatus.h"
#include "EntityStatus.h"
#include "util/CircularBuffer.h"

class QPushButton;
class GLWindow;
class GLInfoDisplay;
class EntityControl;
class TrackControl;
class ScreenAnimationControl;
class BodyControl;
class SoundControl;
class AboutBox;
class LicensePresenter;

class ControlPanel final : public QWidget {
  Q_OBJECT

public:
  explicit ControlPanel(LicensePresenter *presenter);
  ~ControlPanel() noexcept final;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void DoneBodyAnimation();
#pragma clang diagnostic pop

protected:
  void closeEvent(QCloseEvent *e) final;

private:
  // Worker thread, must be initialized very first
  QThread m_WorkerThread;

  // status
  EntityStatus m_EntityStatus;
  wgc0310::HeadStatus m_HeadStatus;
  wgc0310::ScreenAnimationStatus m_ScreenAnimationStatus;
  wgc0310::ScreenDisplayMode m_ScreenDisplayMode;
  wgc0310::BodyStatus m_BodyStatus;
  cw::CircularBuffer<qreal, 160> m_VolumeLevels;
  bool m_VolumeLevelsUpdated;

  // widgets
  GLWindow *m_GLWindow;
  GLInfoDisplay *m_GLInfoDisplay;
  EntityControl *m_EntityControl;
  TrackControl *m_TrackControl;
  ScreenAnimationControl *m_ScreenAnimationControl;
  BodyControl *m_BodyControl;
  SoundControl *m_SoundControl;
  AboutBox *m_AboutBox;

  QPushButton *m_OpenGLSettingsButton;
  QPushButton *m_CameraSettingsButton;
  QPushButton *m_BodyAnimationButton;
  QPushButton *m_FaceAnimationButton;
  QPushButton *m_PoseEstimationButton;
  QPushButton *m_VolumeAnalysisButton;
  QPushButton *m_AboutButton;
};

#endif // PROJECT_WG_UINEXT_CONTROL_PANEL_H
