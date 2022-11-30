#ifndef PROJECT_WG_UINEXT_CONTROL_PANEL_H
#define PROJECT_WG_UINEXT_CONTROL_PANEL_H

#include <QWidget>

class QPushButton;
class GLWidget;

class ControlPanel : public QWidget {
  Q_OBJECT

public:
  ControlPanel();

private:
  GLWidget *m_GLWidget;

  QPushButton *m_OpenGLSettingsButton;
  QPushButton *m_CameraSettingsButton;
  QPushButton *m_BodyAnimationButton;
  QPushButton *m_FaceAnimationButton;
  QPushButton *m_PoseEstimationButton;
  QPushButton *m_AboutButton;
};

#endif // PROJECT_WG_UINEXT_CONTROL_PANEL_H
