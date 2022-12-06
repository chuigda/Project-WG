#include "ui_next/EntityControl.h"

#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QLayout>
#include <QSpinBox>
#include <QPushButton>
#include "include/ui_next/EntityStatus.h"

static QLayout* CreateSliderSuite(QString const& suiteName,
                                  EntityControl *cameraControl,
                                  GLfloat *controlledValue,
                                  int min,
                                  int max,
                                  int stepValue) {
  QHBoxLayout *layout = new QHBoxLayout();
  QLabel *nameLabel = new QLabel(suiteName);
  nameLabel->setFixedWidth(32);

  QSlider *slider = new QSlider();
  slider->setOrientation(Qt::Horizontal);
  slider->setMinimum(min / stepValue);
  slider->setMaximum(max / stepValue);

  QLabel *valueLabel = new QLabel("0");
  valueLabel->setFixedWidth(48);
  QObject::connect(
    slider,
    &QSlider::valueChanged,
    valueLabel,
    [valueLabel, controlledValue, stepValue] (int value) {
      *controlledValue = static_cast<GLfloat>(value * stepValue);
      valueLabel->setText(QString::number(value * stepValue));
    }
  );

  QObject::connect(
    cameraControl,
    &EntityControl::ResetCameraEntityStatus,
    slider,
    [slider, controlledValue, stepValue] () {
      slider->setValue(static_cast<int>(*controlledValue) / stepValue);
    }
  );

  layout->addWidget(nameLabel);
  layout->addWidget(slider);
  layout->addWidget(valueLabel);
  return layout;
}

EntityControl::EntityControl(EntityStatus *status) noexcept
  : m_CameraEntityStatus(status)
{
  setWindowTitle("相机设置");

  this->setMinimumWidth(320);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  this->setLayout(mainLayout);

  {
    QGroupBox *cameraGroupBox = new QGroupBox("位移");
    QVBoxLayout *layout = new QVBoxLayout();
    cameraGroupBox->setLayout(layout);
    QLayout *x = CreateSliderSuite("X", this, &m_CameraEntityStatus->translateX, -100, 100, 5);
    QLayout *y = CreateSliderSuite("Y", this, &m_CameraEntityStatus->translateY, 0, 150, 5);
    QLayout *z = CreateSliderSuite("Z", this, &m_CameraEntityStatus->translateZ, 0, 150, 5);
    layout->addLayout(x);
    layout->addLayout(y);
    layout->addLayout(z);

    mainLayout->addWidget(cameraGroupBox);
  }

  {
    QGroupBox *entityGroupBox = new QGroupBox("旋转");
    QVBoxLayout *layout = new QVBoxLayout();
    entityGroupBox->setLayout(layout);
    QLayout *x = CreateSliderSuite("X", this, &m_CameraEntityStatus->entityRotateX, -360, 360, 10);
    QLayout *y = CreateSliderSuite("Y", this, &m_CameraEntityStatus->entityRotateY, -360, 360, 10);
    QLayout *z = CreateSliderSuite("Z", this, &m_CameraEntityStatus->entityRotateZ, -360, 360, 10);
    layout->addLayout(x);
    layout->addLayout(y);
    layout->addLayout(z);

    mainLayout->addWidget(entityGroupBox);
  }

  {
    QHBoxLayout *resetBox = new QHBoxLayout();
    resetBox->addStretch();
    QPushButton *pushButton = new QPushButton("重置");
    resetBox->addWidget(pushButton);
    mainLayout->addLayout(resetBox);

    connect(
      pushButton,
      &QPushButton::clicked,
      this,
      [this] {
        this->m_CameraEntityStatus->Reset();
        emit this->ResetCameraEntityStatus();
      }
    );
  }

  emit this->ResetCameraEntityStatus();
}
