#include <QApplication>

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>

static QDoubleSpinBox *createAngleSpinBox() {
  QDoubleSpinBox *ret = new QDoubleSpinBox();
  ret->setValue(0.0);
  ret->setMinimum(-45.0);
  ret->setMaximum(45.0);
  return ret;
}

class ConfigWidget final : public QWidget {
public:
  ConfigWidget() {
    setWindowTitle("Project-WG 配置工具");

    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    // 渲染配置
    {
      QGroupBox *groupBox = new QGroupBox("渲染");
      layout->addWidget(groupBox);

      QVBoxLayout *vBox = new QVBoxLayout();
      groupBox->setLayout(vBox);

      QCheckBox *multisampling = new QCheckBox("启用多重采样抗锯齿");
      vBox->addWidget(multisampling);

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("多重采样倍率"));
        hBox->addStretch();

        QRadioButton *msaa2 = new QRadioButton("2x");
        QRadioButton *msaa4 = new QRadioButton("4x");
        QRadioButton *msaa8 = new QRadioButton("8x");
        hBox->addWidget(msaa2);
        hBox->addWidget(msaa4);
        hBox->addWidget(msaa8);
      }

      QCheckBox *lineSmooth = new QCheckBox("启用线条平滑");
      vBox->addWidget(lineSmooth);

      QCheckBox *anisotropyFilter = new QCheckBox("启用各向异性过滤");
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
        QLineEdit *lineEdit = new QLineEdit("11573");
        lineEdit->setFixedWidth(64);
        hBox->addWidget(lineEdit);
      }

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("平滑"));
        hBox->addStretch();
        QLineEdit *lineEdit = new QLineEdit("8");
        lineEdit->setFixedWidth(64);
        hBox->addWidget(lineEdit);
      }

      {
        QHBoxLayout *hBox = new QHBoxLayout();
        vBox->addLayout(hBox);

        hBox->addWidget(new QLabel("校正"));
        hBox->addStretch();

        hBox->addWidget(new QLabel("X"));
        QDoubleSpinBox *xSpin = createAngleSpinBox();
        xSpin->setFixedWidth(64);
        hBox->addWidget(xSpin);

        hBox->addWidget(new QLabel("Y"));
        QDoubleSpinBox *ySpin = createAngleSpinBox();
        ySpin->setFixedWidth(64);
        hBox->addWidget(ySpin);

        hBox->addWidget(new QLabel("Z"));
        QDoubleSpinBox *zSpin = createAngleSpinBox();
        zSpin->setFixedWidth(64);
        hBox->addWidget(zSpin);
      }
    }

    {
      QHBoxLayout *hBox = new QHBoxLayout();
      layout->addLayout(hBox);

      QPushButton *resetButton = new QPushButton("重置");
      QPushButton *saveButton = new QPushButton("保存");

      hBox->addWidget(resetButton);
      hBox->addStretch();
      hBox->addWidget(saveButton);
    }
  }
};

int main(int argc, char *argv[]) {
  QApplication a { argc, argv };
  QApplication::setWindowIcon(QIcon(QPixmap(":/icon-v2-config.png")));

  ConfigWidget w;
  QSize sizeHint = w.minimumSizeHint();
  w.setFixedWidth(sizeHint.width() + 60);
  w.setFixedHeight(sizeHint.height());
  w.show();

  return QApplication::exec();
}
