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
  vtsMode->setEnabled(false);
  vtsMode->setCheckable(false);
  vtsMode->setChecked(false);
  modeSelectBox->addWidget(vtsMode);

  QRadioButton *osfMode = new QRadioButton("OSF");
  osfMode->setChecked(true);
  modeSelectBox->addWidget(osfMode);

  QRadioButton *manualMode = new QRadioButton("手动控制");
  modeSelectBox->addWidget(manualMode);

  mainLayout->addLayout(modeSelectBox);

  m_VTSTrackControl = new VTSTrackControl(m_HeadStatus, m_WorkerThread);
  m_VTSTrackControl->setVisible(false);
  mainLayout->addWidget(m_VTSTrackControl);

  m_OSFTrackControl = new OSFTrackControl(m_HeadStatus, m_WorkerThread);
  mainLayout->addWidget(m_OSFTrackControl);

  m_ManualTrackControl = new ManualTrackControl(m_HeadStatus, m_ScreenDisplayMode);
  m_ManualTrackControl->setVisible(false);
  mainLayout->addWidget(m_ManualTrackControl);

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
    }
  );
}
