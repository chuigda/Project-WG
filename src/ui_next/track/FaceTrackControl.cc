#include "ui_next/FaceTrackControl.h"

#include <QBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include "TrackControlImpl.h"

TrackControl::TrackControl(wgc0310::HeadStatus *headStatus,
                           wgc0310::ScreenDisplayMode *screenDisplayMode,
                           QThread *workerThread)
  : CloseSignallingWidget(nullptr, Qt::Window),
    m_HeadStatus(headStatus),
    m_ScreenDisplayMode(screenDisplayMode),
    m_WorkerThread(workerThread)
{
  this->setWindowTitle("姿态控制");

  QVBoxLayout *mainLayout = new QVBoxLayout();
  this->setLayout(mainLayout);

  QHBoxLayout *modeSelectBox = new QHBoxLayout();
  modeSelectBox->addWidget(new QLabel("模式选择"));
  modeSelectBox->addStretch();

  QRadioButton *vtsMode = new QRadioButton("VTS");
  vtsMode->setChecked(true);
  modeSelectBox->addWidget(vtsMode);

  QRadioButton *osfMode = new QRadioButton("OSF");
  modeSelectBox->addWidget(osfMode);

  QRadioButton *mediaPipeMode = new QRadioButton("MediaPipe");
  modeSelectBox->addWidget(mediaPipeMode);

  QRadioButton *manualMode = new QRadioButton("手动控制");
  modeSelectBox->addWidget(manualMode);

  mainLayout->addLayout(modeSelectBox);

  m_VTSTrackControl = new VTSTrackControl(m_HeadStatus, m_WorkerThread);
  mainLayout->addWidget(m_VTSTrackControl);

  m_OSFTrackControl = new OSFTrackControl(m_HeadStatus, m_WorkerThread);
  m_OSFTrackControl->setVisible(false);
  mainLayout->addWidget(m_OSFTrackControl);

  m_ManualTrackControl = new ManualTrackControl(m_HeadStatus, m_ScreenDisplayMode);
  m_ManualTrackControl->setVisible(false);
  mainLayout->addWidget(m_ManualTrackControl);

  connect(
    vtsMode,
    &QRadioButton::toggled,
    this,
    [this] (bool toggled) {
      if (toggled) {
        m_VTSTrackControl->show();
      } else {
        m_VTSTrackControl->StopTracking();
        m_VTSTrackControl->hide();
      }
      setFixedSize(minimumSizeHint());
    }
  );

  connect(
    osfMode,
    &QRadioButton::toggled,
    this,
    [this] (bool toggled) {
      if (toggled) {
        m_OSFTrackControl->show();
      } else {
        m_OSFTrackControl->StopTracking();
        m_OSFTrackControl->hide();
      }
      setFixedSize(minimumSizeHint());
    }
  );

  connect(
    manualMode,
    &QRadioButton::toggled,
    this,
    [this] (bool toggled) {
      if (toggled) {
        m_ManualTrackControl->show();
      } else {
        m_ManualTrackControl->hide();
      }
      setFixedSize(minimumSizeHint());
    }
  );

#pragma clang diagnostic push
#pragma ide diagnostic ignored "VirtualCallInCtorOrDtor"
  // This should be safe since base class `QWidget` has been initialised,
  // and we did not reimplement `sizeHint`
  setFixedSize(sizeHint());
#pragma clang diagnostic pop
}
