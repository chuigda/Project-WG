#include "ui/ConfigWidget.h"
#include "ui_ConfigWidget.h"
#include "ui/CaptureWidget.h"

void ConfigWidget::SetupFakeCapturePage(FaceTrackStatus *faceTrackStatus) {
  QVBoxLayout *layout = new QVBoxLayout();
  ui->faceCaptureTab->setLayout(layout);

  QHBoxLayout *layout1 = new QHBoxLayout();
  CaptureWidget *captureWidget = new CaptureWidget(faceTrackStatus);
  layout1->addStretch();
  layout1->addWidget(captureWidget);
  layout1->addStretch();

  layout->addStretch();
  layout->addLayout(layout1);
  layout->addStretch();
}

