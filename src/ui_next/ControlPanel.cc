#include "ui_next/ControlPanel.h"

#include <QCloseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include "ui_next/GLWindow.h"
#include "ui_next/GLInfoDisplay.h"
#include "ui_next/EntityControl.h"
#include "ui_next/FaceTrackControl.h"
#include "ui_next/ScreenAnimationControl.h"
#include "ui_next/BodyControl.h"
#include "ui_next/SoundControl.h"
#include "ui_next/AboutBox.h"

static void LinkButtonAndWidget(QPushButton *button, CloseSignallingWidget *widget) {
  button->setCheckable(true);
  QObject::connect(button, &QPushButton::toggled, widget, &QWidget::setVisible);
  QObject::connect(widget, &CloseSignallingWidget::AboutToClose, button, [button] {
    button->setChecked(false);
  });
}

ControlPanel::ControlPanel(LicensePresenter *presenter)
  : QWidget(nullptr, Qt::Window),
    m_ScreenDisplayMode(wgc0310::ScreenDisplayMode::CapturedExpression),
    m_VolumeLevels(0.0),
    m_VolumeLevelsUpdated(false),
    m_GLWindow(new GLWindow(
      &m_EntityStatus,
      &m_HeadStatus,
      &m_BodyStatus,
      &m_ScreenAnimationStatus,
      &m_VolumeLevels,
      &m_VolumeLevelsUpdated,
      &m_ScreenDisplayMode
    )),
    m_GLInfoDisplay(new GLInfoDisplay(m_GLWindow)),
    m_EntityControl(new EntityControl(&m_EntityStatus)),
    m_TrackControl(new TrackControl(&m_HeadStatus, &m_ScreenDisplayMode, &m_WorkerThread)),
    m_ScreenAnimationControl(new ScreenAnimationControl(
      m_GLWindow,
      &m_ScreenAnimationStatus,
      &m_ScreenDisplayMode
    )),
    m_BodyControl(new BodyControl(&m_BodyStatus, this)),
    m_SoundControl(new SoundControl(&m_VolumeLevels, &m_VolumeLevelsUpdated, &m_WorkerThread)),
    m_AboutBox(new AboutBox(presenter)),
    m_OpenGLSettingsButton(new QPushButton("OpenGL")),
    m_CameraSettingsButton(new QPushButton("物体位置")),
    m_BodyAnimationButton(new QPushButton("关节动画")),
    m_FaceAnimationButton(new QPushButton("屏幕画面")),
    m_PoseEstimationButton(new QPushButton("姿态控制")),
    m_VolumeAnalysisButton(new QPushButton("音频分析")),
    m_AboutButton(new QPushButton("关于"))
{
  setWindowTitle("控制面板");

  m_WorkerThread.start();
  m_GLWindow->show();

  QTimer *timer = new QTimer(this);
  timer->setTimerType(Qt::PreciseTimer);
  timer->start();
  connect(timer, &QTimer::timeout, this, [this] {
    if (m_BodyStatus.playAnimationStatus.IsPlayingAnimation()) {
      if (!m_BodyStatus.playAnimationStatus.NextTick(&m_BodyStatus)) {
        m_BodyStatus.playAnimationStatus.SetAnimation(nullptr);
        emit DoneBodyAnimation();
      }
    }

    m_ScreenAnimationStatus.NextTick();
    m_BodyStatus.NextTick();
    m_GLWindow->update();
  });

  LinkButtonAndWidget(m_OpenGLSettingsButton, m_GLInfoDisplay);
  LinkButtonAndWidget(m_CameraSettingsButton, m_EntityControl);
  LinkButtonAndWidget(m_FaceAnimationButton, m_ScreenAnimationControl);
  LinkButtonAndWidget(m_PoseEstimationButton, m_TrackControl);
  LinkButtonAndWidget(m_BodyAnimationButton, m_BodyControl);
  LinkButtonAndWidget(m_VolumeAnalysisButton, m_SoundControl);
  LinkButtonAndWidget(m_AboutButton, m_AboutBox);

  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(m_OpenGLSettingsButton);
  layout->addWidget(m_CameraSettingsButton);
  layout->addWidget(m_PoseEstimationButton);
  layout->addWidget(m_BodyAnimationButton);
  layout->addWidget(m_FaceAnimationButton);
  layout->addWidget(m_VolumeAnalysisButton);
  layout->addWidget(m_AboutButton);
  layout->addStretch();

  this->setLayout(layout);
}

ControlPanel::~ControlPanel() noexcept {
  m_WorkerThread.quit();
  m_WorkerThread.wait();
}

void ControlPanel::closeEvent(QCloseEvent *e) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  int r = QMessageBox::warning(this,
                               "警告 ",
                               "如果关闭这个窗口，将会退出整个程序，确定吗？",
                               "我手滑了",
                               "JUST DO IT!");
#pragma clang diagnostic pop

  if (r == 1) {
    m_GLWindow->close();
    m_GLInfoDisplay->close();
    m_EntityControl->close();
    m_TrackControl->close();
    m_ScreenAnimationControl->close();
    m_BodyControl->close();
    m_SoundControl->close();
    m_AboutBox->close();

    e->accept();
  } else {
    e->ignore();
  }
}