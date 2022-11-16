#include "ui/ConfigWidget.h"
#include "include/wgc0310/BodyStatus.h"
#include "ui_ConfigWidget.h"

#include <QSlider>
#include <QRadioButton>

static void SetupArmGroup(ConfigWidget *configWidget,
                          QGroupBox *groupBox,
                          wgc0310::ArmStatus* armStatus,
                          QPushButton *resetButton);

static QSlider*
AddSlider(QLayout *layout, float *valueSlot, int min, int max, int curr);

void ConfigWidget::SetupBodyPage() {
  connect(ui->radioBlue, &QRadioButton::toggled, [=] (bool checked) {
    if (checked) {
      m_BodyStatus->colorTimerStatus = wgc0310::BodyStatus::Blue;
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
      m_BodyStatus->colorTimerStatus = wgc0310::BodyStatus::Gray;
    }
  });

  connect(ui->blinkTimeSlider, &QSlider::valueChanged, [=] (int value) {
    m_BodyStatus->SetBlinkFrames(value);
  });

  SetupArmGroup(this,
                ui->leftArmGroupBox,
                &m_BodyStatus->leftArmStatus,
                ui->resetBoneButton);
  SetupArmGroup(this,
                ui->rightArmGroupBox,
                &m_BodyStatus->rightArmStatus,
                ui->resetBoneButton);

  connect(ui->resetBoneButton, &QPushButton::clicked, [=] {
    emit this->DoneBodyAnimation();
    m_BodyStatus->Reset();
  });
}

static void SetupArmGroup(ConfigWidget *configWidget,
                          QGroupBox *groupBox,
                          wgc0310::ArmStatus* armStatus,
                          QPushButton *resetButton) {
  QVBoxLayout *layout = static_cast<QVBoxLayout*>(groupBox->layout());
  QSlider *slider0 = AddSlider(layout, &armStatus->rotation[0], -180, 180, 0);
  QSlider *slider1 = AddSlider(layout, &armStatus->rotation[1], -105, 0, -90);
  QSlider *slider2 = AddSlider(layout, &armStatus->rotation[2], -180, 180, 0);
  QSlider *slider3 = AddSlider(layout, &armStatus->rotation[3], -105, 105, 0);
  QSlider *slider4 = AddSlider(layout, &armStatus->rotation[4], -120, 120, 0);

  QObject::connect(resetButton, &QPushButton::clicked, [=] {
    slider0->setValue(0);
    slider1->setValue(-90 / 5);
    slider2->setValue(0);
    slider3->setValue(0);
    slider4->setValue(0);
  });

  QObject::connect(configWidget, &ConfigWidget::StartBodyAnimation, [=] {
    slider0->setEnabled(false);
    slider1->setEnabled(false);
    slider2->setEnabled(false);
    slider3->setEnabled(false);
    slider4->setEnabled(false);
  });

  QObject::connect(configWidget, &ConfigWidget::DoneBodyAnimation, [=] {
    slider0->setEnabled(true);
    slider1->setEnabled(true);
    slider2->setEnabled(true);
    slider3->setEnabled(true);
    slider4->setEnabled(true);
    slider0->setValue(static_cast<int>(armStatus->rotation[0] / 5.0f));
    slider1->setValue(static_cast<int>(armStatus->rotation[1] / 5.0f));
    slider2->setValue(static_cast<int>(armStatus->rotation[2] / 5.0f));
    slider3->setValue(static_cast<int>(armStatus->rotation[3] / 5.0f));
    slider4->setValue(static_cast<int>(armStatus->rotation[4] / 5.0f));
  });
}

static QSlider*
AddSlider(QLayout *layout, float *valueSlot, int min, int max, int curr) {
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setMinimum(min / 5);
  slider->setMaximum(max / 5);
  slider->setValue(curr / 5);
  layout->addWidget(slider);
  QObject::connect(slider, &QSlider::valueChanged, [=] (int value) {
    *valueSlot = static_cast<float>(value) * 5.0f;
  });
  return slider;
}

void ConfigWidget::OnBodyAnimationsLoaded(
  std::vector<std::unique_ptr<wgc0310::BodyAnimation>> *animations
) {
  QVBoxLayout *layout = new QVBoxLayout();
  ui->presetBodyAnimGroupBox->setLayout(layout);

  for (const auto &animation : *animations) {
    wgc0310::BodyAnimation const* animationPtr = animation.get();
    QPushButton *button = new QPushButton(animationPtr->GetName());
    layout->addWidget(button);

    connect(button, &QPushButton::clicked, [this, animationPtr] {
      this->m_BodyStatus->playAnimationStatus.SetAnimation(animationPtr);
      emit this->StartBodyAnimation();
    });
  }

  layout->addStretch();
}
