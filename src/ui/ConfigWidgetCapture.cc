#include "ui/ConfigWidget.h"
#include "ui_ConfigWidget.h"
#include "ui/ManualOperateWidget.h"
#include "ui/OSFTrack.h"

void ConfigWidget::SetupCapturePage(FaceTrackStatus *faceTrackStatus) {
  QBoxLayout *mainLayout =
      static_cast<QBoxLayout*>(ui->faceCaptureTab->layout());

  QWidget *helperWidget = new QWidget;

  QVBoxLayout *layout = new QVBoxLayout();
  QHBoxLayout *layout1 = new QHBoxLayout();
  ManualOperateWidget *manualOperateWidget
    = new ManualOperateWidget(faceTrackStatus);
  layout1->addStretch();
  layout1->addWidget(manualOperateWidget);
  layout1->addStretch();

  layout->addStretch();
  layout->addLayout(layout1);
  layout->addStretch();

  helperWidget->setLayout(layout);
  helperWidget->setVisible(false);
  mainLayout->addWidget(helperWidget);

  OSFTrackController *osfTrackController
    = new OSFTrackController(faceTrackStatus);
  mainLayout->addWidget(osfTrackController);

  mainLayout->addStretch();

  connect(
    ui->radioOSFCapture,
    &QRadioButton::toggled,
    [helperWidget, osfTrackController] (bool toggled) {
      if (toggled) {
        helperWidget->setVisible(false);
        osfTrackController->setVisible(true);
      } else {
        helperWidget->setVisible(true);
        osfTrackController->setVisible(false);
        emit osfTrackController->StopListening();
      }
    }
  );
}
