#include "ui_next/ExtraControl.h"

#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
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

static QDoubleSpinBox *CreateAPSpinBox(GLfloat *linkedValue,
                                       GLfloat min,
                                       GLfloat max,
                                       GLfloat value,
                                       GLfloat step) {
  QDoubleSpinBox *ret = new QDoubleSpinBox();
  ret->setEnabled(false);
  ret->setMinimum(min);
  ret->setMaximum(max);
  ret->setDecimals(4);
  ret->setSingleStep(step);
  ret->setValue(value);
  QObject::connect(ret, &QDoubleSpinBox::valueChanged, ret, [linkedValue] (double value) {
    *linkedValue = static_cast<GLfloat>(value);
  });
  return ret;
}

ExtraControl::ExtraControl(GLWindow *glWindow, StatusExtra *statusExtra)
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

  // 着色器
  {
    QGroupBox *group = new QGroupBox("着色器选择");
    layout->addWidget(group);
    QVBoxLayout *vBox = new QVBoxLayout();
    group->setLayout(vBox);

    QRadioButton *phong = new QRadioButton("冯氏着色器 (Phong)");
    phong->setChecked(true);
    phong->setToolTip("Project-WG 从 0.1.0 开始默认使用的着色器");
    vBox->addWidget(phong);

    QRadioButton *gouraud = new QRadioButton("高氏着色器 (Gouraud)");
    gouraud->setToolTip("用顶点着色器进行光线计算，然后进行插值\r\n"
                        "因为处理的样本总数更少，可能会有更好的性能，但是效果较差");
    vBox->addWidget(gouraud);

    QRadioButton *phongWithNormalTex = new QRadioButton("冯氏着色器 + 纹理");
    phongWithNormalTex->setToolTip("在冯氏着色器的基础上使用纹理增加细节");
    phongWithNormalTex->setEnabled(false);
    vBox->addWidget(phongWithNormalTex);

    QRadioButton *pbr = new QRadioButton("基于物理规则渲染 (PBR)");
    pbr->setToolTip("模拟真实世界的物理规则进行渲染，需要强劲的电脑");
    pbr->setEnabled(false);
    vBox->addWidget(pbr);

    connect(phong, &QRadioButton::toggled, m_GLWindow, [this] (bool toggled) {
      if (toggled) {
        m_GLWindow->RunWithGLContext([this] {
          m_GLWindow->SetShader(0);
        });
      }
    });

    connect(gouraud, &QRadioButton::toggled, m_GLWindow, [this] (bool toggled) {
      if (toggled) {
        m_GLWindow->RunWithGLContext([this] {
          m_GLWindow->SetShader(1);
        });
      }
    });
  }

  // 描边
  {
    QGroupBox *groupBox = new QGroupBox("描边");
    layout->addWidget(groupBox);

    QVBoxLayout *vBox = new QVBoxLayout();
    groupBox->setLayout(vBox);

    QCheckBox *strokeEnabled = new QCheckBox("启用描边");
    strokeEnabled->setToolTip("为模型绘制描边，增加细节的同时缓解模型在浅色背景上看不清的问题");
    vBox->addWidget(strokeEnabled);

    QHBoxLayout *hBox = new QHBoxLayout();
    vBox->addLayout(hBox);
    hBox->addWidget(new QLabel("描边颜色"));
    hBox->addStretch();

    QSpinBox *red = CreateColorSpinBox(&m_StatusExtra->strokeColor.r);
    QSpinBox *green = CreateColorSpinBox(&m_StatusExtra->strokeColor.g);
    QSpinBox *blue = CreateColorSpinBox(&m_StatusExtra->strokeColor.b);
    hBox->addWidget(new QLabel("R"));
    hBox->addWidget(red);
    hBox->addWidget(new QLabel("G"));
    hBox->addWidget(green);
    hBox->addWidget(new QLabel("B"));
    hBox->addWidget(blue);

    hBox = new QHBoxLayout();
    vBox->addLayout(hBox);

    hBox->addWidget(new QLabel("描边宽度"));
    hBox->addStretch();
    QDoubleSpinBox *strokeWidth = CreateAPSpinBox(&m_StatusExtra->strokeLineWidth,
                                                  0.0f,
                                                  5.0f,
                                                  1.0f,
                                                  0.0005f);
    hBox->addWidget(strokeWidth);

    hBox = new QHBoxLayout();
    vBox->addLayout(hBox);
    QLabel *adjustment = new QLabel("微调");
    adjustment->setToolTip("将描边整体放大，避免描边显示不完全");
    hBox->addWidget(adjustment);
    hBox->addStretch();
    QDoubleSpinBox *strokeAdjustment = CreateAPSpinBox(&m_StatusExtra->strokeAdjustment,
                                                       1.0f,
                                                       1.01f,
                                                       1.001f,
                                                       0.0001f);
    hBox->addWidget(strokeAdjustment);

    connect(strokeEnabled,
            &QCheckBox::toggled,
            this,
            [this, red, green, blue, strokeWidth, strokeAdjustment] (bool toggled) {
              m_StatusExtra->stroke = toggled;
              red->setEnabled(toggled);
              green->setEnabled(toggled);
              blue->setEnabled(toggled);
              strokeWidth->setEnabled(toggled);
              strokeAdjustment->setEnabled(toggled);
            });
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
