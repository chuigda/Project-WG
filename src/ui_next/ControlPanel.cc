#include "ui_next/ControlPanel.h"
#include "ui/GLWidget.h"

#include <QPushButton>
#include <QHBoxLayout>
#include "ui_next/GLWindow.h"
#include "ui_next/CameraControl.h"
#include "ui_next/FaceTrackControl.h"
#include "ui_next/ScreenAnimationControl.h"
#include "ui_next/CloseSignallingWidget.h"

static void LinkButtonAndWidget(QPushButton *button, CloseSignallingWidget *widget) {
  QObject::connect(button, &QPushButton::toggled, widget, [widget] (bool checked) {
    if (checked) {
      widget->show();
    } else {
      widget->hide();
    }
  });
  QObject::connect(widget, &CloseSignallingWidget::AboutToClose, button, [button] {
    button->setChecked(false);
  });
}

ControlPanel::ControlPanel()
  : CloseSignallingWidget(nullptr, Qt::Window),
    m_ScreenDisplayMode(wgc0310::ScreenDisplayMode::CapturedExpression),
    m_GLWindow(new GLWindow(
      &m_CameraEntityStatus,
      &m_HeadStatus,
      nullptr,
      nullptr,
      &m_ScreenDisplayMode
    )),
    m_CameraControl(new CameraControl(&m_CameraEntityStatus)),
    m_TrackControl(new TrackControl(&m_HeadStatus, &m_ScreenDisplayMode, &m_WorkerThread)),
    m_ScreenAnimationControl(new ScreenAnimationControl(m_GLWindow, &m_ScreenAnimationStatus)),
    m_OpenGLSettingsButton(new QPushButton("OpenGL 设定")),
    m_CameraSettingsButton(new QPushButton("相机设置")),
    m_BodyAnimationButton(new QPushButton("关节动画")),
    m_FaceAnimationButton(new QPushButton("面部动画")),
    m_PoseEstimationButton(new QPushButton("姿态控制")),
    m_AboutButton(new QPushButton("关于"))
{
  setWindowTitle("控制面板");

  m_WorkerThread.start();
  m_GLWindow->show();

  m_OpenGLSettingsButton->setCheckable(true);
  m_CameraSettingsButton->setCheckable(true);
  m_BodyAnimationButton->setCheckable(true);
  m_FaceAnimationButton->setCheckable(true);
  m_PoseEstimationButton->setCheckable(true);
  m_AboutButton->setCheckable(true);

  LinkButtonAndWidget(m_CameraSettingsButton, m_CameraControl);
  LinkButtonAndWidget(m_FaceAnimationButton, m_ScreenAnimationControl);
  LinkButtonAndWidget(m_PoseEstimationButton, m_TrackControl);

  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(m_OpenGLSettingsButton);
  layout->addWidget(m_CameraSettingsButton);
  layout->addWidget(m_BodyAnimationButton);
  layout->addWidget(m_FaceAnimationButton);
  layout->addWidget(m_PoseEstimationButton);
  layout->addWidget(m_AboutButton);
  layout->addStretch();

  this->setLayout(layout);
}

ControlPanel::~ControlPanel() noexcept {
  m_WorkerThread.quit();
  m_WorkerThread.wait();
}
