#include "ui_next/CameraControl.h"

#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QLayout>
#include <QSpinBox>
#include <QPushButton>
#include "ui/CameraEntityStatus.h"

static QLayout* CreateSliderSuite(QString const& suiteName,
                                  CameraControl *cameraControl,
                                  GLfloat *controlledValue) {
  QHBoxLayout *layout = new QHBoxLayout();
  QLabel *nameLabel = new QLabel(suiteName);

  QSlider *slider = new QSlider();
  slider->setOrientation(Qt::Horizontal);
  slider->setMinimum(-360 / 5);
  slider->setMaximum(360 / 5);

  QLabel *valueLabel = new QLabel("0");
  valueLabel->setFixedWidth(48);
  QObject::connect(
    slider,
    &QSlider::valueChanged,
    valueLabel,
    [valueLabel, controlledValue] (int value) {
      *controlledValue = static_cast<GLfloat>(value) * 5.0f;
      valueLabel->setText(QString::number(value * 5));
    }
  );

  QObject::connect(
    cameraControl,
    &CameraControl::ResetCameraEntityStatus,
    slider,
    [slider, cameraControl, controlledValue] () {
      cameraControl->blockSignals(true);
      slider->setValue(static_cast<int>(*controlledValue / 5.0f));
      cameraControl->blockSignals(false);
    }
  );

  layout->addWidget(nameLabel);
  layout->addWidget(slider);
  layout->addWidget(valueLabel);
  return layout;
}

CameraControl::CameraControl(CameraEntityStatus *status) noexcept
  : m_CameraEntityStatus(status)
{
  QVBoxLayout *mainLayout = new QVBoxLayout();
  this->setLayout(mainLayout);

  {
    QGroupBox *cameraGroupBox = new QGroupBox("相机参数");
    QVBoxLayout *layout = new QVBoxLayout();
    cameraGroupBox->setLayout(layout);
    QLayout *x = CreateSliderSuite("X 位移", this, &m_CameraEntityStatus->cameraX);
    QLayout *y = CreateSliderSuite("Y 位移", this, &m_CameraEntityStatus->cameraY);
    QLayout *z = CreateSliderSuite("Z 位移", this, &m_CameraEntityStatus->cameraZ);
    layout->addLayout(x);
    layout->addLayout(y);
    layout->addLayout(z);

    mainLayout->addWidget(cameraGroupBox);
  }

  {
    QGroupBox *entityGroupBox = new QGroupBox("物体参数");
    QVBoxLayout *layout = new QVBoxLayout();
    entityGroupBox->setLayout(layout);
    QLayout *x = CreateSliderSuite("X 旋转", this, &m_CameraEntityStatus->entityRotateX);
    QLayout *y = CreateSliderSuite("Y 旋转", this, &m_CameraEntityStatus->entityRotateY);
    QLayout *z = CreateSliderSuite("Z 旋转", this, &m_CameraEntityStatus->entityRotateZ);
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
