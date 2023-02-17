#include "ui_next/ExtraControl.h"

#include <QLabel>
#include <QSpinBox>
#include <QBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>

#include "cwglx/GL/GLImpl.h"
#include "ui_next/GLWindow.h"

static QSpinBox *CreateColorSpinBox(GLfloat *linkedValue) {
  QSpinBox *ret = new QSpinBox();
  ret->setEnabled(false);
  ret->setMinimum(0);
  ret->setMaximum(255);
  QObject::connect(ret, &QSpinBox::valueChanged, ret, [linkedValue] (int value) {
    *linkedValue = static_cast<GLfloat>(value) / 255.0f;
  });
  return ret;
}

ExtraControl::ExtraControl(GLWindow *glWindow,
                           StatusExtra *statusExtra)
  : m_GLWindow(glWindow),
    m_StatusExtra(statusExtra)
{
  setWindowTitle("附加选项");

  QVBoxLayout *layout = new QVBoxLayout();
  this->setLayout(layout);

  // 常规设置
  {
    QGroupBox *groupBox = new QGroupBox("常规");
    layout->addWidget(groupBox);
    QVBoxLayout *vBox = new QVBoxLayout();
    groupBox->setLayout(vBox);

    QCheckBox *stayOnTopBox = new QCheckBox("所有窗口常居顶端");
    vBox->addWidget(stayOnTopBox);
    stayOnTopBox->setChecked(false);
    stayOnTopBox->setToolTip("尝试让所有窗口常居顶端，具体效果通常取决于操作系统");

    connect(stayOnTopBox, &QCheckBox::toggled, this, [this](bool toggled) {
      m_StatusExtra->stayOnTop = toggled;
      emit this->SetStayOnTop(toggled);
    });

    {
      QHBoxLayout *hBox = new QHBoxLayout();
      vBox->addLayout(hBox);

      QCheckBox *customClearColorEnabled = new QCheckBox("自定义背景色");
      customClearColorEnabled->setToolTip("不推荐。建议使用透明背景 + 游戏捕获");
      QSpinBox *red = CreateColorSpinBox(&m_StatusExtra->clearColor.r);
      QSpinBox *green = CreateColorSpinBox(&m_StatusExtra->clearColor.g);
      QSpinBox *blue = CreateColorSpinBox(&m_StatusExtra->clearColor.b);

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
        [this, red, green, blue](bool toggled) {
          m_StatusExtra->customClearColor = toggled;
          red->setEnabled(toggled);
          green->setEnabled(toggled);
          blue->setEnabled(toggled);
        }
      );
    }
  }

  // 抗锯齿与线条平滑
  {
    QGroupBox *groupBox = new QGroupBox("抗锯齿");
    layout->addWidget(groupBox);

    QVBoxLayout *vBox = new QVBoxLayout();
    groupBox->setLayout(vBox);

    QCheckBox *multisample = new QCheckBox("多重采样抗锯齿");
    multisample->setToolTip("启用或关闭基于 <code>GL_MULTISAMPLE</code> 的多重采样抗锯齿");
    multisample->setChecked(true);

    QCheckBox *lineSmooth = new QCheckBox("线条平滑");
    lineSmooth->setToolTip("启用或关闭基于 <code>GL_LINE_SMOOTH</code> 的线条平滑<br/>");
    lineSmooth->setChecked(true);

    vBox->addWidget(multisample);
    vBox->addWidget(lineSmooth);

    connect(multisample, &QCheckBox::toggled, this, [this](bool toggled) {
      m_GLWindow->RunWithGLContext([this, toggled] {
        if (toggled) {
          m_GLWindow->GL->glEnable(GL_MULTISAMPLE);
        } else {
          m_GLWindow->GL->glDisable(GL_MULTISAMPLE);
        }
      });
    });

    connect(lineSmooth, &QCheckBox::toggled, this, [this](bool toggled) {
      m_GLWindow->RunWithGLContext([this, toggled] {
        if (toggled) {
          m_GLWindow->GL->glEnable(GL_LINE_SMOOTH);
        } else {
          m_GLWindow->GL->glDisable(GL_LINE_SMOOTH);
        }
      });
    });
  }

  {
    QGroupBox *groupBox = new QGroupBox("调试");
    layout->addWidget(groupBox);

    QVBoxLayout *vBox = new QVBoxLayout();
    groupBox->setLayout(vBox);

    QPushButton *reloadModelButton = new QPushButton("重新加载模型");
    connect(reloadModelButton, &QPushButton::clicked, m_GLWindow, [this] {
      m_GLWindow->RunWithGLContext([this] {
        m_GLWindow->ReloadModel();
      });
    });
    vBox->addWidget(reloadModelButton);
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "VirtualCallInCtorOrDtor"
  // This should be safe since base class `QWidget` has been initialised,
  // and we did not reimplement `sizeHint`
  QSize size = sizeHint();
#pragma clang diagnostic pop
  setFixedWidth(size.width() + 60);
  setFixedHeight(size.height());
}
