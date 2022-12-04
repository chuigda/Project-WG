#include "ui_next/FaceTrackControl.h"

#include <QBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include "TrackControlImpl.h"

TrackControl::TrackControl(wgc0310::HeadStatus *headStatus,
                           wgc0310::ScreenDisplayMode *screenDisplayMode,
                           QThread *workerThread)
  : QWidget(nullptr, Qt::Window),
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

  QRadioButton *manualMode = new QRadioButton("Manual");
  modeSelectBox->addWidget(manualMode);

  mainLayout->addLayout(modeSelectBox);

  VTSTrackControl *vtsTrackControl = new VTSTrackControl(m_HeadStatus, m_WorkerThread);
  vtsTrackControl->setVisible(false);
  mainLayout->addWidget(vtsTrackControl);

  OSFTrackControl *osfTrackControl = new OSFTrackControl(m_HeadStatus, m_WorkerThread);
  mainLayout->addWidget(osfTrackControl);

  ManualTrackControl *manualTrackControl =
    new ManualTrackControl(m_HeadStatus, m_ScreenDisplayMode);
  manualTrackControl->setVisible(false);
  mainLayout->addWidget(manualTrackControl);

  connect(
    osfMode,
    &QRadioButton::toggled,
    this,
    [osfTrackControl] (bool toggled) {
      if (toggled) {
        osfTrackControl->show();
      } else {
        osfTrackControl->StopTracking();
        osfTrackControl->hide();
      }
    }
  );

  connect(
    manualMode,
    &QRadioButton::toggled,
    this,
    [manualTrackControl] (bool toggled) {
      if (toggled) {
        manualTrackControl->show();
      } else {
        manualTrackControl->hide();
      }
    }
  );
}
