#include <QApplication>

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QMessageBox>

#include "GlobalConfig.h"
#include "util/FileUtil.h"

static QDoubleSpinBox *createAngleSpinBox() {
  QDoubleSpinBox *ret = new QDoubleSpinBox();
  ret->setMinimum(-45.0);
  ret->setMaximum(45.0);
  return ret;
}

static QSpinBox *createColorSpinBox() {
  QSpinBox *ret = new QSpinBox();
  ret->setMinimum(0);
  ret->setMaximum(255);
  return ret;
}

class ConfigWidget final : public QWidget {
public:
  ConfigWidget() {
    setWindowTitle("Project-WG 配置工具");

    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    // 通用配置
    {
      QGroupBox *groupBox = new QGroupBox("通用");
      layout->addWidget(groupBox);

      QVBoxLayout *vBox = new QVBoxLayout();
      groupBox->setLayout(vBox);

      QCheckBox *stayOnTop = new QCheckBox("所有窗口长居顶端");
      stayOnTop->setChecked(cw::GlobalConfig::Instance.stayOnTop);
      connect(stayOnTop, &QCheckBox::toggled, this, [] (bool enabled) {
        cw::GlobalConfig::Instance.stayOnTop = enabled;
      });
      vBox->addWidget(stayOnTop);

      QCheckBox *fillBackground = new QCheckBox("背景填充");
      fillBackground->setChecked(cw::GlobalConfig::Instance.fillBackground);
      connect(fillBackground, &QCheckBox::toggled, this, [] (bool enabled) {
        cw::GlobalConfig::Instance.fillBackground = enabled;
      });
      vBox->addWidget(fillBackground);

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("背景填充颜色"));
        hBox->addStretch();

        QSpinBox *r = createColorSpinBox();
        QSpinBox *g = createColorSpinBox();
        QSpinBox *b = createColorSpinBox();
        r->setValue(static_cast<int>(cw::GlobalConfig::Instance.backgroundColor.r));
        g->setValue(static_cast<int>(cw::GlobalConfig::Instance.backgroundColor.g));
        b->setValue(static_cast<int>(cw::GlobalConfig::Instance.backgroundColor.b));
        connect(r, &QSpinBox::valueChanged, this, [] (int value) {
          cw::GlobalConfig::Instance.backgroundColor.r = static_cast<float>(value);
        });
        connect(g, &QSpinBox::valueChanged, this, [] (int value) {
          cw::GlobalConfig::Instance.backgroundColor.g = static_cast<float>(value);
        });
        connect(b, &QSpinBox::valueChanged, this, [] (int value) {
          cw::GlobalConfig::Instance.backgroundColor.b = static_cast<float>(value);
        });

        hBox->addWidget(new QLabel("R"));
        hBox->addWidget(r);
        hBox->addWidget(new QLabel("G"));
        hBox->addWidget(g);
        hBox->addWidget(new QLabel("B"));
        hBox->addWidget(b);
      }
    }

    // 渲染配置
    {
      QGroupBox *groupBox = new QGroupBox("渲染");
      layout->addWidget(groupBox);

      QVBoxLayout *vBox = new QVBoxLayout();
      groupBox->setLayout(vBox);

      QCheckBox *multisampling = new QCheckBox("启用多重采样抗锯齿");
      multisampling->setChecked(cw::GlobalConfig::Instance.multisampling);
      connect(multisampling, &QCheckBox::toggled, this, [] (bool enabled) {
        cw::GlobalConfig::Instance.multisampling = enabled;
      });
      vBox->addWidget(multisampling);

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("多重采样倍率"));
        hBox->addStretch();

        QRadioButton *msaa2 = new QRadioButton("2x");
        QRadioButton *msaa4 = new QRadioButton("4x");
        QRadioButton *msaa8 = new QRadioButton("8x");

        switch (cw::GlobalConfig::Instance.multisamplingSamples) {
          case 8:
            msaa8->setChecked(true);
            break;

          case 4:
            msaa4->setChecked(true);
            break;

          case 2:
          default:
            msaa2->setChecked(true);
            break;
        }

        connect(msaa2, &QRadioButton::toggled, this, [] (bool checked) {
          if (checked) {
            cw::GlobalConfig::Instance.multisamplingSamples = 2;
          }
        });

        connect(msaa4, &QRadioButton::toggled, this, [] (bool checked) {
          if (checked) {
            cw::GlobalConfig::Instance.multisamplingSamples = 4;
          }
        });

        connect(msaa8, &QRadioButton::toggled, this, [] (bool checked) {
          if (checked) {
            cw::GlobalConfig::Instance.multisamplingSamples = 8;
          }
        });

        hBox->addWidget(msaa2);
        hBox->addWidget(msaa4);
        hBox->addWidget(msaa8);
      }

      QCheckBox *lineSmooth = new QCheckBox("启用线条平滑");
      lineSmooth->setChecked(cw::GlobalConfig::Instance.lineSmoothHint);
      connect(lineSmooth, &QCheckBox::toggled, this, [] (bool enabled) {
        cw::GlobalConfig::Instance.lineSmoothHint = enabled;
      });
      vBox->addWidget(lineSmooth);

      QCheckBox *anisotropyFilter = new QCheckBox("启用各向异性过滤");
      anisotropyFilter->setChecked(cw::GlobalConfig::Instance.anisotropyFilter);
      connect(anisotropyFilter, &QCheckBox::toggled, this, [] (bool enabled) {
        cw::GlobalConfig::Instance.anisotropyFilter = enabled;
      });
      vBox->addWidget(anisotropyFilter);
    }

    // 控制器配置
    {
      QGroupBox *groupBox = new QGroupBox("控制");
      layout->addWidget(groupBox);

      QVBoxLayout *vBox = new QVBoxLayout();
      groupBox->setLayout(vBox);

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("默认控制模式"));
        hBox->addStretch();

        QRadioButton *none = new QRadioButton("无");
        QRadioButton *vts = new QRadioButton("VTS");
        QRadioButton *osf = new QRadioButton("OSF");
        QRadioButton *mp = new QRadioButton("MediaPipe");
        QRadioButton *manual = new QRadioButton("手动");

        switch (cw::GlobalConfig::Instance.defaultControlMode) {
          case cw::GlobalConfig::ControlMode::None:
            none->setChecked(true);
            break;
          case cw::GlobalConfig::ControlMode::VTS:
            vts->setChecked(true);
            break;
          case cw::GlobalConfig::ControlMode::OSF:
            osf->setChecked(true);
            break;
          case cw::GlobalConfig::ControlMode::MP:
            mp->setChecked(true);
            break;
          case cw::GlobalConfig::ControlMode::Manual:
            manual->setChecked(true);
            break;
        }

        connect(none, &QRadioButton::toggled, this, [] (bool checked) {
          if (checked) {
            cw::GlobalConfig::Instance.defaultControlMode = cw::GlobalConfig::ControlMode::None;
          }
        });

        connect(vts, &QRadioButton::toggled, this, [] (bool checked) {
          if (checked) {
            cw::GlobalConfig::Instance.defaultControlMode = cw::GlobalConfig::ControlMode::VTS;
          }
        });

        connect(osf, &QRadioButton::toggled, this, [] (bool checked) {
          if (checked) {
            cw::GlobalConfig::Instance.defaultControlMode = cw::GlobalConfig::ControlMode::OSF;
          }
        });

        connect(mp, &QRadioButton::toggled, this, [] (bool checked) {
          if (checked) {
            cw::GlobalConfig::Instance.defaultControlMode = cw::GlobalConfig::ControlMode::MP;
          }
        });

        connect(manual, &QRadioButton::toggled, this, [] (bool checked) {
          if (checked) {
            cw::GlobalConfig::Instance.defaultControlMode = cw::GlobalConfig::ControlMode::Manual;
          }
        });

        hBox->addWidget(none);
        hBox->addWidget(vts);
        hBox->addWidget(osf);
        hBox->addWidget(mp);
        hBox->addWidget(manual);
      }
    }

    // VTS 配置
    {
      QGroupBox *groupBox = new QGroupBox("VTS");
      layout->addWidget(groupBox);

      QHBoxLayout *hBox = new QHBoxLayout();
      groupBox->setLayout(hBox);

      hBox->addWidget(new QLabel("WebSocket 端口"));
      hBox->addStretch();
      QLineEdit *lineEdit = new QLineEdit("8001");
      lineEdit->setFixedWidth(64);
      lineEdit->setText(QString::number(cw::GlobalConfig::Instance.vtsWebsocketPort));
      connect(lineEdit, &QLineEdit::textChanged, this, [] (const QString &text) {
        cw::GlobalConfig::Instance.vtsWebsocketPort = text.toInt();
      });
      hBox->addWidget(lineEdit);
    }

    // OSF 配置
    {
      QGroupBox *groupBox = new QGroupBox("OSF");
      layout->addWidget(groupBox);

      QVBoxLayout *vBox = new QVBoxLayout();
      groupBox->setLayout(vBox);

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("UDP 端口"));
        hBox->addStretch();
        QLineEdit *lineEdit = new QLineEdit();
        lineEdit->setFixedWidth(64);
        lineEdit->setText(QString::number(cw::GlobalConfig::Instance.osfUdpPort));
        connect(lineEdit, &QLineEdit::textChanged, this, [] (const QString &text) {
          cw::GlobalConfig::Instance.osfUdpPort = text.toInt();
        });
        hBox->addWidget(lineEdit);
      }

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("平滑"));
        hBox->addStretch();
        QLineEdit *lineEdit = new QLineEdit();
        lineEdit->setFixedWidth(64);
        lineEdit->setText(QString::number(cw::GlobalConfig::Instance.osfSmooth));
        connect(lineEdit, &QLineEdit::textChanged, this, [] (const QString &text) {
          cw::GlobalConfig::Instance.osfSmooth = text.toInt();
        });
        hBox->addWidget(lineEdit);
      }

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("校正"));
        hBox->addStretch();

        QDoubleSpinBox *xSpin = createAngleSpinBox();
        xSpin->setFixedWidth(64);
        xSpin->setValue(cw::GlobalConfig::Instance.osfCorrectionX);
        connect(xSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [] (double value) {
          cw::GlobalConfig::Instance.osfCorrectionX = static_cast<float>(value);
        });

        QDoubleSpinBox *ySpin = createAngleSpinBox();
        ySpin->setFixedWidth(64);
        ySpin->setValue(cw::GlobalConfig::Instance.osfCorrectionY);
        connect(ySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [] (double value) {
          cw::GlobalConfig::Instance.osfCorrectionY = static_cast<float>(value);
        });

        QDoubleSpinBox *zSpin = createAngleSpinBox();
        zSpin->setFixedWidth(64);
        zSpin->setValue(cw::GlobalConfig::Instance.osfCorrectionZ);
        connect(zSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [] (double value) {
          cw::GlobalConfig::Instance.osfCorrectionZ = static_cast<float>(value);
        });

        hBox->addWidget(new QLabel("X"));
        hBox->addWidget(xSpin);
        hBox->addWidget(new QLabel("Y"));
        hBox->addWidget(ySpin);
        hBox->addWidget(new QLabel("Z"));
        hBox->addWidget(zSpin);
      }
    }

    {
      QHBoxLayout *hBox = new QHBoxLayout();
      layout->addLayout(hBox);

      QPushButton *saveButton = new QPushButton("保存");

      hBox->addStretch();
      hBox->addWidget(saveButton);

      connect(saveButton, &QPushButton::clicked, this, [this] {
        QMessageBox::StandardButton button =
          QMessageBox::information(this,
                                   "确认",
                                   "确定要保存并覆盖当前的配置吗？",
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);
        if (button == QMessageBox::No) {
          return;
        }

        cw::WriteToFile("config.ini", QStringLiteral(R"abc123([common]
# 所有窗口常居顶端
stay_on_top=%1
# 启用背景填充
fill_background=%2
# 自定义背景颜色
background_r=%3
background_g=%4
background_b=%5

[render]
# 多重采样抗锯齿
multisampling=%6
# 多重采样抗锯齿样本数
multisampling_samples=%7
# 线条平滑
line_smooth=%8
# 各向异性过滤
anisotropy_filter=%9

[control]
# 默认模式
default_mode=%10

[control.vts]
# WebSocket 端口
websocket_port=%11

[control.osf]
# UDP 端口
udp_port=%12
# XYZ 校正
correction_x=%13
correction_y=%14
correction_z=%15
# 平滑
smooth=%16
)abc123")
          // common
          .arg(cw::GlobalConfig::Instance.stayOnTop ? "true" : "false")
          .arg(cw::GlobalConfig::Instance.fillBackground ? "true" : "false")
          .arg(cw::GlobalConfig::Instance.backgroundColor.r)
          .arg(cw::GlobalConfig::Instance.backgroundColor.g)
          .arg(cw::GlobalConfig::Instance.backgroundColor.b)
          // render
          .arg(cw::GlobalConfig::Instance.multisampling ? "true" : "false")
          .arg(cw::GlobalConfig::Instance.multisamplingSamples)
          .arg(cw::GlobalConfig::Instance.lineSmoothHint ? "true" : "false")
          .arg(cw::GlobalConfig::Instance.anisotropyFilter ? "true" : "false")
          // control
          .arg(cw::GlobalConfig::ControlModeToString(cw::GlobalConfig::Instance.defaultControlMode))
          // control.vts
          .arg(cw::GlobalConfig::Instance.vtsWebsocketPort)
          // control.osf
          .arg(cw::GlobalConfig::Instance.osfUdpPort)
          .arg(cw::GlobalConfig::Instance.osfCorrectionX)
          .arg(cw::GlobalConfig::Instance.osfCorrectionY)
          .arg(cw::GlobalConfig::Instance.osfCorrectionZ)
          .arg(cw::GlobalConfig::Instance.osfSmooth)
        );
      });
    }
  }
};

int main(int argc, char *argv[]) {
  cw::InitGlobalConfig();

  QApplication a { argc, argv };
  QApplication::setWindowIcon(QIcon(QPixmap(":/icon-v2-config.png")));

  ConfigWidget w;
  QSize sizeHint = w.minimumSizeHint();
  w.setFixedWidth(sizeHint.width() + 60);
  w.setFixedHeight(sizeHint.height());
  w.show();

  return QApplication::exec();
}
