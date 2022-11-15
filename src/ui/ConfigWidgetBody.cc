#include "ui/ConfigWidget.h"
#include "ui/BodyStatus.h"
#include "ui_ConfigWidget.h"

#include <QRadioButton>

void ConfigWidget::SetupBodyPage() {
  connect(ui->radioBlue, &QRadioButton::toggled, [=] (bool checked) {
    if (checked) {
      m_BodyStatus->colorTimerStatus = BodyStatus::Blue;
    }
  });

  connect(ui->radioBlink, &QRadioButton::toggled, [=] (bool checked) {
    if (checked) {
      m_BodyStatus->StartBlinking();
      ui->blinkTimeSlider->setEnabled(true);
    } else {
      ui->blinkTimeSlider->setEnabled(false);
    }
  });

  connect(ui->radioGray, &QRadioButton::toggled, [=] (bool checked) {
    if (checked) {
      qDebug() << "gray!";
      m_BodyStatus->colorTimerStatus = BodyStatus::Gray;
    }
  });

  connect(ui->blinkTimeSlider, &QSlider::valueChanged, [=] (int value) {
    m_BodyStatus->SetBlinkFrames(value);
  });
}
