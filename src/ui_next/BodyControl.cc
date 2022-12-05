#include "ui_next/BodyControl.h"
#include "ui_next/ControlPanel.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>

static std::pair<QSlider*, QLayout*>
AddSlider(QLayout *layout, float *valueSlot, int min, int max, int curr) {
  QSlider *slider = new QSlider(Qt::Horizontal);
  slider->setMinimum(min / 5);
  slider->setMaximum(max / 5);
  slider->setValue(curr / 5);
  layout->addWidget(slider);

  QLabel *label = new QLabel(QString::number(curr));
  label->setFixedWidth(48);
  QObject::connect(slider, &QSlider::valueChanged, slider, [=] (int value) {
    *valueSlot = static_cast<float>(value) * 5.0f;
    label->setText(QString::number(*valueSlot));
  });

  QHBoxLayout *retLayout = new QHBoxLayout();
  retLayout->addWidget(slider);
  retLayout->addWidget(label);

  return std::make_pair(slider, retLayout);
}

static void SetupArmGroup(BodyControl *bodyControl,
                          ControlPanel *controlPanel,
                          QVBoxLayout *layout,
                          wgc0310::ArmStatus* armStatus,
                          QPushButton *resetButton) {
  auto [slider0, layout0] = AddSlider(layout, &armStatus->rotation[0], -180, 180, 0);
  auto [slider1, layout1] = AddSlider(layout, &armStatus->rotation[1], -105, 0, -90);
  auto [slider2, layout2] = AddSlider(layout, &armStatus->rotation[2], -180, 180, 0);
  auto [slider3, layout3] = AddSlider(layout, &armStatus->rotation[3], -105, 105, 0);
  auto [slider4, layout4] = AddSlider(layout, &armStatus->rotation[4], -120, 120, 0);

  auto slider0_ = slider0;
  auto slider1_ = slider1;
  auto slider2_ = slider2;
  auto slider3_ = slider3;
  auto slider4_ = slider4;

  QObject::connect(
    resetButton,
    &QPushButton::clicked,
    [slider0_, slider1_, slider2_, slider3_, slider4_] {
      slider0_->setValue(0);
      slider1_->setValue(-90 / 5);
      slider2_->setValue(0);
      slider3_->setValue(0);
      slider4_->setValue(0);
    }
  );

  QObject::connect(
    bodyControl,
    &BodyControl::StartBodyAnimation,
    [slider0_, slider1_, slider2_, slider3_, slider4_] {
      slider0_->setEnabled(false);
      slider1_->setEnabled(false);
      slider2_->setEnabled(false);
      slider3_->setEnabled(false);
      slider4_->setEnabled(false);
    }
  );

  QObject::connect(
    controlPanel,
    &ControlPanel::DoneBodyAnimation,
    [slider0_, slider1_, slider2_, slider3_, slider4_, armStatus] {
      slider0_->setEnabled(true);
      slider1_->setEnabled(true);
      slider2_->setEnabled(true);
      slider3_->setEnabled(true);
      slider4_->setEnabled(true);
      slider0_->setValue(static_cast<int>(armStatus->rotation[0] / 5.0f));
      slider1_->setValue(static_cast<int>(armStatus->rotation[1] / 5.0f));
      slider2_->setValue(static_cast<int>(armStatus->rotation[2] / 5.0f));
      slider3_->setValue(static_cast<int>(armStatus->rotation[3] / 5.0f));
      slider4_->setValue(static_cast<int>(armStatus->rotation[4] / 5.0f));
    }
  );

  layout->addLayout(layout0);
  layout->addLayout(layout1);
  layout->addLayout(layout2);
  layout->addLayout(layout3);
  layout->addLayout(layout4);
}

BodyControl::BodyControl(wgc0310::BodyStatus *bodyStatus,
                         ControlPanel *controlPanel)
  : CloseSignallingWidget(nullptr, Qt::Window),
    m_BodyStatus(bodyStatus)
{
  setWindowTitle("身体控制");

  QVBoxLayout *mainLayout = new QVBoxLayout();
  this->setLayout(mainLayout);

  QPushButton *resetButton = new QPushButton("重置");

  {
    QGroupBox *colorTimerGroup = new QGroupBox("彩色计时器");
    mainLayout->addWidget(colorTimerGroup);
    QVBoxLayout *layout = new QVBoxLayout();
    colorTimerGroup->setLayout(layout);

    {
      QHBoxLayout *radioLayout = new QHBoxLayout();
      layout->addLayout(radioLayout);

      QRadioButton *radioBlue = new QRadioButton("常亮");
      connect(radioBlue, &QRadioButton::toggled, radioBlue, [this](bool toggled) {
        if (toggled) {
          m_BodyStatus->colorTimerStatus = wgc0310::BodyStatus::Blue;
        }
      });
      radioLayout->addWidget(radioBlue);

      QRadioButton *radioBlink = new QRadioButton("闪烁");
      connect(radioBlink, &QRadioButton::toggled, radioBlink, [this](bool toggled) {
        if (toggled) {
          m_BodyStatus->colorTimerStatus = wgc0310::BodyStatus::Blinking;
        }
      });
      radioLayout->addWidget(radioBlink);

      QRadioButton *radioGray = new QRadioButton("熄灭");
      connect(radioGray, &QRadioButton::toggled, radioGray, [this](bool toggled) {
        if (toggled) {
          m_BodyStatus->colorTimerStatus = wgc0310::BodyStatus::Gray;
        }
      });
    }

    {
      QSlider *slider = new QSlider();
      slider->setMinimum(15);
      slider->setMaximum(40);
      slider->setValue(30);
      slider->setOrientation(Qt::Horizontal);
      connect(slider, &QSlider::valueChanged, [=] (int value) {
        m_BodyStatus->SetBlinkFrames(value);
      });
      layout->addWidget(slider);
    }
  }

  {
    QHBoxLayout *hSplit = new QHBoxLayout;
    mainLayout->addLayout(hSplit);

    QGroupBox *leftArm = new QGroupBox("左臂");
    hSplit->addWidget(leftArm);
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftArm->setLayout(leftLayout);
    SetupArmGroup(this,
                  controlPanel,
                  leftLayout,
                  &m_BodyStatus->leftArmStatus,
                  resetButton);

    QGroupBox *rightArm = new QGroupBox("右臂");
    hSplit->addWidget(rightArm);
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightArm->setLayout(rightLayout);
    SetupArmGroup(this,
                  controlPanel,
                  rightLayout,
                  &m_BodyStatus->rightArmStatus,
                  resetButton);
  }

  QHBoxLayout *lastLayout = new QHBoxLayout();
  lastLayout->addStretch();
  lastLayout->addWidget(resetButton);

  mainLayout->addLayout(lastLayout);
}
