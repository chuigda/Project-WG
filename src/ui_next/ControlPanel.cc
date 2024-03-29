#include "ui_next/ControlPanel.h"

#include <QCloseEvent>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QTimer>
#include <QLabel>
#include "ui_next/GLWindow.h"
#include "ui_next/GLInfoDisplay.h"
#include "ui_next/EntityControl.h"
#include "ui_next/FaceTrackControl.h"
#include "ui_next/ScreenAnimationControl.h"
#include "ui_next/BodyControl.h"
#include "ui_next/AttachmentControl.h"
#include "ui_next/SoundControl.h"
#include "ui_next/ShaderEdit.h"
#include "ui_next/HelpBox.h"

static void LinkButtonAndWidget(QPushButton *button, CloseSignallingWidget *widget) {
  button->setCheckable(true);
  QObject::connect(button, &QPushButton::toggled, widget, [widget] (bool toggled) {
    if (toggled) {
      widget->show();
      widget->setWindowState(widget->windowState() & ~Qt::WindowMinimized);
    } else {
      widget->hide();
    }
  });
  QObject::connect(widget, &CloseSignallingWidget::AboutToHideOrClose, button, [button] {
    button->blockSignals(true);
    button->setChecked(false);
    button->blockSignals(false);
  });
}

ControlPanel::ControlPanel(bool startHideGL)
  : QWidget(nullptr, Qt::Window),
    m_ScreenDisplayMode(wgc0310::ScreenDisplayMode::CapturedExpression),
    m_VolumeLevels(0.0),
    m_VolumeLevelsUpdated(false),
    m_StartHideGL(startHideGL),
    m_GLWindow(new GLWindow(
      &m_EntityStatus,
      &m_HeadStatus,
      &m_BodyStatus,
      &m_AttachmentStatus,
      &m_ScreenAnimationStatus,
      &m_VolumeLevels,
      &m_VolumeLevelsUpdated,
      &m_ScreenDisplayMode,
      &m_ExtraStatus
    )),
    m_GLInfoDisplay(new GLInfoDisplay(m_GLWindow)),
    m_EntityControl(new EntityControl(&m_EntityStatus)),
    m_TrackControl(new TrackControl(&m_HeadStatus, &m_ScreenDisplayMode, &m_WorkerThread)),
    m_ScreenAnimationControl(new ScreenAnimationControl(
      m_GLWindow,
      &m_ScreenAnimationStatus,
      &m_ScreenDisplayMode,
      &m_ExtraStatus
    )),
    m_BodyControl(new BodyControl(&m_BodyStatus, this)),
    m_AttachmentControl(new AttachmentControl(&m_AttachmentStatus, m_GLWindow, &m_ExtraStatus)),
    m_SoundControl(new SoundControl(&m_VolumeLevels, &m_VolumeLevelsUpdated, &m_WorkerThread)),
    m_ExtraControl(new ExtraControl(m_GLWindow, &m_ExtraStatus)),
    m_ShaderEdit(new ShaderEdit(m_GLWindow)),
    m_HelpBox(new HelpBox()),
    m_OpenGLSettingsButton(new QPushButton("OpenGL")),
    m_CameraSettingsButton(new QPushButton("物体位置")),
    m_BodyAnimationButton(new QPushButton("关节动画")),
    m_AttachmentButton(new QPushButton("附加配件")),
    m_FaceAnimationButton(new QPushButton("屏幕画面")),
    m_PoseEstimationButton(new QPushButton("姿态控制")),
    m_VolumeAnalysisButton(new QPushButton("音频分析")),
    m_ExtraSettingsButton(new QPushButton("附加选项")),
    m_ShaderEditButton(new QPushButton("着色器")),
    m_HelpButton(new QPushButton("帮助"))
{
  setWindowTitle("控制面板");

  m_WorkerThread.start();

  QTimer *timer = new QTimer(this);
  timer->setTimerType(Qt::PreciseTimer);
  timer->setInterval(1000 / 90);
  timer->start();
  connect(timer, &QTimer::timeout, this, &ControlPanel::NextTick);

  LinkButtonAndWidget(m_OpenGLSettingsButton, m_GLInfoDisplay);
  LinkButtonAndWidget(m_CameraSettingsButton, m_EntityControl);
  LinkButtonAndWidget(m_FaceAnimationButton, m_ScreenAnimationControl);
  LinkButtonAndWidget(m_PoseEstimationButton, m_TrackControl);
  LinkButtonAndWidget(m_BodyAnimationButton, m_BodyControl);
  LinkButtonAndWidget(m_AttachmentButton, m_AttachmentControl);
  LinkButtonAndWidget(m_VolumeAnalysisButton, m_SoundControl);
  LinkButtonAndWidget(m_ExtraSettingsButton, m_ExtraControl);
  LinkButtonAndWidget(m_ShaderEditButton, m_ShaderEdit);
  LinkButtonAndWidget(m_HelpButton, m_HelpBox);

  connect(m_ExtraControl, &ExtraControl::SetStayOnTop, this, [this] (bool stayOnTop) {
    bool glInfo = m_GLInfoDisplay->isVisible();
    bool entityControl = m_EntityControl->isVisible();
    bool trackControl = m_TrackControl->isVisible();
    bool screenAnimationControl = m_ScreenAnimationControl->isVisible();
    bool bodyControl = m_BodyControl->isVisible();
    bool attachmentControl = m_AttachmentControl->isVisible();
    bool soundControl = m_SoundControl->isVisible();
    bool shaderEdit = m_ShaderEdit->isVisible();
    bool helpBox = m_HelpBox->isVisible();

    this->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_GLWindow->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_GLInfoDisplay->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_EntityControl->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_TrackControl->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_ScreenAnimationControl->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_BodyControl->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_AttachmentControl->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_SoundControl->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_ExtraControl->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_ShaderEdit->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);
    m_HelpBox->setWindowFlag(Qt::WindowStaysOnTopHint, stayOnTop);

    this->show();
    m_GLWindow->show();
    if (glInfo) { m_GLInfoDisplay->show(); }
    if (entityControl) { m_EntityControl->show(); }
    if (trackControl) { m_TrackControl->show(); }
    if (screenAnimationControl) { m_ScreenAnimationControl->show(); }
    if (bodyControl) { m_BodyControl->show(); }
    if (attachmentControl) { m_AttachmentControl->show(); }
    if (soundControl) { m_SoundControl->show(); }
    m_ExtraControl->show();
    if (shaderEdit) { m_ShaderEdit->show(); }
    if (helpBox) { m_HelpBox->show(); }
  });

  QGridLayout *layout = new QGridLayout();
  layout->setSpacing(3);
  layout->setContentsMargins(3, 3, 3, 3);
  layout->addWidget(m_OpenGLSettingsButton, 0, 0);
  layout->addWidget(m_CameraSettingsButton, 0, 1);
  layout->addWidget(m_PoseEstimationButton, 0, 2);
  layout->addWidget(m_BodyAnimationButton, 0, 3);
  layout->addWidget(m_AttachmentButton, 0, 4);
  layout->addWidget(m_FaceAnimationButton, 1, 0);
  layout->addWidget(m_VolumeAnalysisButton, 1, 1);
  layout->addWidget(m_ExtraSettingsButton, 1, 2);
  layout->addWidget(m_ShaderEditButton, 1, 3);
  layout->addWidget(m_HelpButton, 1, 4);

  this->setLayout(layout);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "VirtualCallInCtorOrDtor"
  // This should be safe since base class `QWidget` has been initialised,
  // and we did not reimplement `sizeHint`
  setFixedSize(sizeHint());
#pragma clang diagnostic pop
}

ControlPanel::~ControlPanel() noexcept {
  m_WorkerThread.quit();
  m_WorkerThread.wait();
}

void ControlPanel::DoneSplash() {
  if (!m_StartHideGL) {
    m_GLWindow->show();
  }
}

void ControlPanel::closeEvent(QCloseEvent *e) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  int r = QMessageBox::warning(this,
                               "警告",
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
    m_AttachmentControl->close();
    m_SoundControl->close();
    m_ExtraControl->close();
    m_ShaderEdit->close();
    m_HelpBox->close();

    e->accept();
  } else {
    e->ignore();
  }
}

void ControlPanel::NextTick() {
  // if (m_BodyStatus.playAnimationStatus.IsPlayingAnimation()) {
  //   if (!m_BodyStatus.playAnimationStatus.NextTick(&m_BodyStatus)) {
  //     m_BodyStatus.playAnimationStatus.SetAnimation(nullptr);
  //     emit DoneBodyAnimation();
  //   }
  // }
  //
  // m_AttachmentStatus.NextTick();
  // m_ScreenAnimationStatus.NextTick();
  // m_BodyStatus.NextTick();
  m_GLWindow->update();
}
