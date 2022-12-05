#include "ui_next/BodyControl.h"
#include "ui_next/ControlPanel.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QRadioButton>
#include <QLabel>
#include <QPushButton>
#include <QDir>

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
    m_BodyStatus(bodyStatus),
    m_MinimizedLayout(new QHBoxLayout()),
    m_DetailedLayout(new QVBoxLayout())
{
  setWindowTitle("关节动画");
  setMinimumWidth(600);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  this->setLayout(mainLayout);

  QPushButton *resetButton = new QPushButton("重置");

  // 彩色计时器
  {
    QGroupBox *colorTimerGroup = new QGroupBox("彩色计时器");
    mainLayout->addWidget(colorTimerGroup);
    QVBoxLayout *layout = new QVBoxLayout();
    colorTimerGroup->setLayout(layout);

    {
      QSlider *slider = new QSlider();
      slider->setDisabled(true);
      slider->setMinimum(15);
      slider->setMaximum(40);
      slider->setValue(30);
      slider->setOrientation(Qt::Horizontal);

      QHBoxLayout *radioLayout = new QHBoxLayout();
      layout->addLayout(radioLayout);

      QRadioButton *radioBlue = new QRadioButton("常亮");
      radioBlue->setChecked(true);
      connect(radioBlue, &QRadioButton::toggled, radioBlue, [this](bool toggled) {
        if (toggled) {
          m_BodyStatus->colorTimerStatus = wgc0310::BodyStatus::Blue;
        }
      });
      radioLayout->addWidget(radioBlue);

      QRadioButton *radioBlink = new QRadioButton("闪烁");
      connect(radioBlink, &QRadioButton::toggled, radioBlink, [this, slider](bool toggled) {
        if (toggled) {
          slider->setDisabled(false);
          m_BodyStatus->colorTimerStatus = wgc0310::BodyStatus::Blinking;
        } else {
          slider->setDisabled(true);
        }
      });
      radioLayout->addWidget(radioBlink);

      QRadioButton *radioGray = new QRadioButton("熄灭");
      connect(radioGray, &QRadioButton::toggled, radioGray, [this](bool toggled) {
        if (toggled) {
          m_BodyStatus->colorTimerStatus = wgc0310::BodyStatus::Gray;
        }
      });
      radioLayout->addWidget(radioGray);

      connect(slider, &QSlider::valueChanged, [=] (int value) {
        m_BodyStatus->SetBlinkFrames(value);
      });
      layout->addWidget(slider);
    }
  }

  QGroupBox *groupBoxMinimized = new QGroupBox("动画列表");
  QGroupBox *groupBoxDetailed = new QGroupBox("动画列表");
  QWidget *armStatusWidget = new QWidget();

  groupBoxDetailed->setVisible(false);
  armStatusWidget->setVisible(false);
  armStatusWidget->setVisible(false);

  mainLayout->addWidget(groupBoxMinimized);
  mainLayout->addWidget(groupBoxDetailed);
  mainLayout->addWidget(armStatusWidget);

  // 动画列表，最小化状态
  {
    QHBoxLayout *layout = new QHBoxLayout();
    groupBoxMinimized->setLayout(layout);

    layout->addLayout(m_MinimizedLayout);
    layout->addStretch();

    QPushButton *reloadButton = new QPushButton("↻");
    reloadButton->setFixedWidth(32);
    layout->addWidget(reloadButton);
    connect(reloadButton, &QPushButton::clicked, this, [this] {
      ReloadBodyAnimations();
    });

    QPushButton *expandButton = new QPushButton("▼");
    expandButton->setFixedWidth(32);
    layout->addWidget(expandButton);
    connect(
      expandButton,
      &QPushButton::clicked,
      this,
      [groupBoxMinimized, groupBoxDetailed, armStatusWidget] {
        groupBoxMinimized->setVisible(false);
        groupBoxDetailed->setVisible(true);
        armStatusWidget->setVisible(true);
      }
    );
  }

  // 动画列表，常规状态
  {
    QVBoxLayout *layout = new QVBoxLayout();
    groupBoxDetailed->setLayout(layout);

    layout->addLayout(m_DetailedLayout);
    layout->addStretch();

    QPushButton *reloadButton = new QPushButton("重新加载动画");
    layout->addWidget(reloadButton);
    connect(reloadButton, &QPushButton::clicked, this, [this] {
      ReloadBodyAnimations();
    });

    QPushButton *retractButton = new QPushButton("收起");
    layout->addWidget(retractButton);
    connect(
      retractButton,
      &QPushButton::clicked,
      this,
      [=] {
        groupBoxMinimized->setVisible(true);
        groupBoxDetailed->setVisible(false);
        armStatusWidget->setVisible(false);
      }
    );
  }

  // 胳膊状态，可以隐藏
  {
    QHBoxLayout *hSplit = new QHBoxLayout;
    armStatusWidget->setLayout(hSplit);
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

  ReloadBodyAnimations();
}

static void ClearLayout(QLayout *layout) {
  while (QLayoutItem *child = layout->takeAt(0))  {
    delete child->widget();
    delete child;
  }
}

void BodyControl::ReloadBodyAnimations() {
  m_BodyAnimations.clear();
  ClearLayout(m_MinimizedLayout);
  ClearLayout(m_DetailedLayout);

  QDir dir(QStringLiteral("animations/body"));
  QStringList filters;
  filters << "*.anim" << "*.txt" << "*.cfg";

  QStringList files = dir.entryList(filters, QDir::Files);
  for (const auto &file : files) {
    std::unique_ptr<wgc0310::BodyAnimation> animation =
      wgc0310::LoadBodyAnimation(
        (QStringLiteral("animations/body/") + file).toStdString().c_str()
      );
    if (!animation) {
      continue;
    }

    m_BodyAnimations.push_back(std::move(animation));
  }
}
