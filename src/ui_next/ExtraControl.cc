#include "ui_next/ExtraControl.h"

#include <QLabel>
#include <QSpinBox>
#include <QBoxLayout>
#include <QCheckBox>

static QSpinBox *CreateColorSpinBox(std::uint8_t *linkedValue) {
  QSpinBox *ret = new QSpinBox();
  ret->setEnabled(false);
  ret->setMinimum(0);
  ret->setMaximum(255);
  QObject::connect(ret, &QSpinBox::valueChanged, ret, [linkedValue] (int value) {
    *linkedValue = static_cast<std::uint8_t>(value);
  });
  return ret;
}

ExtraControl::ExtraControl(StatusExtra *statusExtra)
  : m_StatusExtra(statusExtra)
{
  setWindowTitle("附加选项");

  QVBoxLayout *layout = new QVBoxLayout();
  this->setLayout(layout);

  QCheckBox *stayOnTopBox = new QCheckBox("所有窗口常居顶端");
  layout->addWidget(stayOnTopBox);
  stayOnTopBox->setChecked(false);
  stayOnTopBox->setToolTip("尝试让所有窗口常居顶端，此功能的效果取决于窗口管理器");

  connect(stayOnTopBox, &QCheckBox::toggled, this, [this] (bool toggled) {
    m_StatusExtra->stayOnTop = toggled;
    emit this->SetStayOnTop(toggled);
  });

  {
    QHBoxLayout *hBox = new QHBoxLayout();
    layout->addLayout(hBox);

    QCheckBox *customClearColorEnabled = new QCheckBox("自定义背景色");
    customClearColorEnabled->setToolTip("不推荐使用，建议使用透明背景 + 游戏捕获");
    std::array<std::uint8_t, 4>& colorArray = m_StatusExtra->clearColor.GetRawRepr();
    QSpinBox *red = CreateColorSpinBox(&colorArray[0]);
    QSpinBox *green = CreateColorSpinBox(&colorArray[1]);
    QSpinBox *blue = CreateColorSpinBox(&colorArray[2]);

    hBox->addWidget(customClearColorEnabled);
    hBox->addStretch();
    hBox->addWidget(new QLabel("R"));
    hBox->addWidget(red);
    hBox->addWidget(new QLabel("G"));
    hBox->addWidget(green);
    hBox->addWidget(new QLabel("B"));
    hBox->addWidget(blue);

    connect(
      customClearColorEnabled,
      &QCheckBox::toggled,
      this,
      [this, red, green, blue] (bool toggled) {
        m_StatusExtra->customClearColor = toggled;
        red->setEnabled(toggled);
        green->setEnabled(toggled);
        blue->setEnabled(toggled);
      }
    );
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "VirtualCallInCtorOrDtor"
  // This should be safe since base class `QWidget` has been initialised,
  // and we did not reimplement `sizeHint`
  setFixedSize(sizeHint());
#pragma clang diagnostic pop
}
