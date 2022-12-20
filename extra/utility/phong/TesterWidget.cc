#include "TesterWidget.h"

#include <vector>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QTimer>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "cwglx/MeshLoader.h"

static QDoubleSpinBox *CreateColorSpinBox(QHBoxLayout *layout) {
  QDoubleSpinBox *ret = new QDoubleSpinBox();
  ret->setMinimum(0.0f);
  ret->setMaximum(1.0f);
  ret->setSingleStep(0.01f);
  layout->addWidget(ret);
  return ret;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-make-unique"
TestMesh::TestMesh() {
  teapotMesh = cw::LoadMesh("./model/teapot.mesh", 7.5);

  monitorMesh = cw::LoadMesh("./model/monitor.mesh");
  monitorIntakeMesh = cw::LoadMesh("./model/monitor-intake.mesh");

  chestBoxMesh = cw::LoadMesh("./model/chest-box.mesh");
  chestPlateMesh = cw::LoadMesh("./model/chest-plate.mesh");

  bigArmMesh = cw::LoadMesh("./model/big-arm.mesh");
  bigArmCoverMesh = cw::LoadMesh("./model/big-arm-cover.mesh");

  smallArmMesh = cw::LoadMesh("./model/small-arm.mesh");
  smallArmCoverMesh = cw::LoadMesh("./model/small-arm-cover.mesh");

  clawMesh = cw::LoadMesh("./model/claw.mesh");
  clawCoverMesh = cw::LoadMesh("./model/claw-cover.mesh");

  headMesh.reset(new cw::Composition(
    std::vector<cw::TranslationStep> {},
    std::vector<cw::Drawable const*> { monitorMesh.get(), monitorIntakeMesh.get() }
  ));

  chestMesh.reset(new cw::Composition(
    std::vector<cw::TranslationStep> {},
    std::vector<cw::Drawable const*> { chestBoxMesh.get(), chestPlateMesh.get() }
  ));

  bigArmFullMesh.reset(new cw::Composition(
    std::vector<cw::TranslationStep> {},
    std::vector<cw::Drawable const*> { bigArmMesh.get(), bigArmCoverMesh.get() }
  ));

  smallArmFullMesh.reset(new cw::Composition(
    std::vector<cw::TranslationStep> {},
    std::vector<cw::Drawable const*> { smallArmMesh.get(), smallArmCoverMesh.get() }
  ));

  clawFullMesh.reset(new cw::Composition(
    std::vector<cw::TranslationStep> {},
    std::vector<cw::Drawable const*> { clawMesh.get(), clawCoverMesh.get() }
  ));
}

void TestMesh::Delete(GLFunctions *f) {
  teapotMesh->Delete(f);
  monitorMesh->Delete(f);
  monitorIntakeMesh->Delete(f);
  chestBoxMesh->Delete(f);
  chestPlateMesh->Delete(f);
  bigArmMesh->Delete(f);
  bigArmCoverMesh->Delete(f);
  smallArmMesh->Delete(f);
  smallArmCoverMesh->Delete(f);
  clawMesh->Delete(f);
  clawCoverMesh->Delete(f);
}

#pragma clang diagnostic pop

TesterWidget::TesterWidget()
  : m_MeshCollection(),
    m_CustomMaterial(*cw::GetBrassMaterial()),
    m_CurrentMesh(m_MeshCollection.teapotMesh.get()),
    m_CurrentMaterial(cw::GetBrassMaterial()),
    m_PaintWidget(new PaintWidget(&m_EntityStatus, &m_CurrentMesh, &m_CurrentMaterial))
{
  m_EntityStatus.translateY = 0;

  setWindowTitle("PHONG 光照模型调试器");

  QVBoxLayout *layout = new QVBoxLayout();
  setLayout(layout);

  QHBoxLayout *paintLayout = new QHBoxLayout();
  layout->addLayout(paintLayout);
  paintLayout->addWidget(m_PaintWidget);

  QSlider *slider = new QSlider(Qt::Vertical);
  paintLayout->addWidget(slider);
  slider->setMinimum(-10);
  slider->setMaximum(10);
  slider->setSingleStep(1);
  connect(slider, &QSlider::valueChanged, this, [this] (int value) {
    m_EntityStatus.translateY = static_cast<GLfloat>(value) * 5.0f;
  });

  {
    QHBoxLayout *meshChooseLayout = new QHBoxLayout();
    layout->addLayout(meshChooseLayout);

    QLabel *meshChooseLabel = new QLabel("选择模型");
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("OpenGL 茶壶");
    comboBox->addItem("头部");
    comboBox->addItem("胸部");
    comboBox->addItem("大臂");
    comboBox->addItem("小臂");
    comboBox->addItem("手部");

    connect(comboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
      switch (index) {
        case 0: m_CurrentMesh = m_MeshCollection.teapotMesh.get(); break;
        case 1: m_CurrentMesh = m_MeshCollection.headMesh.get(); break;
        case 2: m_CurrentMesh = m_MeshCollection.chestMesh.get(); break;
        case 3: m_CurrentMesh = m_MeshCollection.bigArmFullMesh.get(); break;
        case 4: m_CurrentMesh = m_MeshCollection.smallArmFullMesh.get(); break;
        case 5: m_CurrentMesh = m_MeshCollection.clawFullMesh.get(); break;

        default: Q_UNREACHABLE();
      }
    });

    meshChooseLayout->addWidget(meshChooseLabel);
    meshChooseLayout->addWidget(comboBox);
  }

  {
    QGroupBox *groupBox = new QGroupBox("材料设置");
    layout->addWidget(groupBox);

    QGridLayout *gridLayout = new QGridLayout();
    groupBox->setLayout(gridLayout);

    QRadioButton *radioStandard = new QRadioButton("从标准材料选择");
    radioStandard->setChecked(true);
    QRadioButton *radioCustom = new QRadioButton("自定义材料");
    QComboBox *comboBox = new QComboBox();

    gridLayout->addWidget(radioStandard, 0, 0);
    gridLayout->addWidget(comboBox, 0, 1);
    gridLayout->addWidget(radioCustom, 1, 0);

    QWidget *customMaterialWidget = new QWidget();
    gridLayout->addWidget(customMaterialWidget, 1, 1);

    QVBoxLayout *customMaterialLayout = new QVBoxLayout();
    customMaterialWidget->setLayout(customMaterialLayout);
    customMaterialWidget->setEnabled(false);

    QHBoxLayout *ambientLayout = new QHBoxLayout();
    customMaterialLayout->addLayout(ambientLayout);
    ambientLayout->addWidget(new QLabel("环境光"));
    ambientLayout->addStretch();
    QDoubleSpinBox *ambientR = CreateColorSpinBox(ambientLayout);
    QDoubleSpinBox *ambientG = CreateColorSpinBox(ambientLayout);
    QDoubleSpinBox *ambientB = CreateColorSpinBox(ambientLayout);

    QHBoxLayout *diffuseLayout = new QHBoxLayout();
    customMaterialLayout->addLayout(diffuseLayout);
    diffuseLayout->addWidget(new QLabel("漫反射光"));
    diffuseLayout->addStretch();
    QDoubleSpinBox *diffuseR = CreateColorSpinBox(diffuseLayout);
    QDoubleSpinBox *diffuseG = CreateColorSpinBox(diffuseLayout);
    QDoubleSpinBox *diffuseB = CreateColorSpinBox(diffuseLayout);

    QHBoxLayout *specularLayout = new QHBoxLayout();
    customMaterialLayout->addLayout(specularLayout);
    specularLayout->addWidget(new QLabel("镜面反射光"));
    specularLayout->addStretch();
    QDoubleSpinBox *specularR = CreateColorSpinBox(specularLayout);
    QDoubleSpinBox *specularG = CreateColorSpinBox(specularLayout);
    QDoubleSpinBox *specularB = CreateColorSpinBox(specularLayout);

    QHBoxLayout *lastLayout = new QHBoxLayout();
    customMaterialLayout->addLayout(lastLayout);
    lastLayout->addWidget(new QLabel("镜面反射强度"));
    QDoubleSpinBox *shininess = new QDoubleSpinBox();
    shininess->setMinimum(0.0f);
    shininess->setMaximum(128.0f);
    shininess->setSingleStep(0.1f);
    lastLayout->addWidget(shininess);
    lastLayout->addStretch();
    QPushButton *confirmButton = new QPushButton("确认");
    lastLayout->addWidget(confirmButton);

    auto applySelectedMaterial = [=, this] (int index) {
      switch (index) {
        case 0: m_CurrentMaterial = cw::GetBrassMaterial(); break;
        case 1: m_CurrentMaterial = cw::GetChromeMaterial(); break;
        case 2: m_CurrentMaterial = cw::GetTinMaterial(); break;
        case 3: m_CurrentMaterial = cw::GetPlasticMaterial(); break;

        case 4: m_CurrentMaterial = cw::GetBlackPlasticMaterial(); break;
        case 5: m_CurrentMaterial = cw::GetSteel15N20Material(); break;

        case 6: m_CurrentMaterial = cw::GetBronzeMaterial(); break;
        case 7: m_CurrentMaterial = cw::GetPolishedBronzeMaterial(); break;
        case 8: m_CurrentMaterial = cw::GetCobreMaterial(); break;
        case 9: m_CurrentMaterial = cw::GetPolishedCobreMaterial(); break;
        case 10: m_CurrentMaterial = cw::GetGoldMaterial(); break;
        case 11: m_CurrentMaterial = cw::GetPolishedGoldMaterial(); break;

        default: Q_UNREACHABLE();
      }

      ambientR->setValue(m_CurrentMaterial->ambient.GetRed());
      ambientG->setValue(m_CurrentMaterial->ambient.GetGreen());
      ambientB->setValue(m_CurrentMaterial->ambient.GetBlue());

      diffuseR->setValue(m_CurrentMaterial->diffuse.GetRed());
      diffuseG->setValue(m_CurrentMaterial->diffuse.GetGreen());
      diffuseB->setValue(m_CurrentMaterial->diffuse.GetBlue());

      specularR->setValue(m_CurrentMaterial->specular.GetRed());
      specularG->setValue(m_CurrentMaterial->specular.GetGreen());
      specularB->setValue(m_CurrentMaterial->specular.GetBlue());

      shininess->setValue(m_CurrentMaterial->shininess);
    };

    {
      comboBox->addItem("(标准) 黄铜");
      comboBox->addItem("(标准) 铬");
      comboBox->addItem("(标准) 锡");
      comboBox->addItem("(标准) 塑料");

      comboBox->addItem("(自定) 黑色塑料");
      comboBox->addItem("(自定) 15N20钢");

      comboBox->addItem("(不推荐) 青铜");
      comboBox->addItem("(不推荐) 磨过的青铜");
      comboBox->addItem("(不推荐) 铜");
      comboBox->addItem("(不推荐) 磨过的铜");
      comboBox->addItem("(不推荐) 金");
      comboBox->addItem("(不推荐) 磨过的金");

      connect(comboBox, &QComboBox::currentIndexChanged,
              this, applySelectedMaterial);
    }

    connect(
      radioStandard,
      &QRadioButton::toggled,
      comboBox,
      [comboBox, applySelectedMaterial] (bool checked) {
        comboBox->setEnabled(checked);
        if (checked) {
          applySelectedMaterial(comboBox->currentIndex());
        }
      }
    );

    connect(radioCustom, &QRadioButton::toggled,
            customMaterialWidget, &QWidget::setEnabled);

    connect(
      confirmButton,
      &QPushButton::clicked,
      this,
      [=, this] {
        m_CustomMaterial.ambient.SetRed(static_cast<GLfloat>(ambientR->value()));
        m_CustomMaterial.ambient.SetGreen(static_cast<GLfloat>(ambientG->value()));
        m_CustomMaterial.ambient.SetBlue(static_cast<GLfloat>(ambientB->value()));

        m_CustomMaterial.diffuse.SetRed(static_cast<GLfloat>(diffuseR->value()));
        m_CustomMaterial.diffuse.SetGreen(static_cast<GLfloat>(diffuseG->value()));
        m_CustomMaterial.diffuse.SetBlue(static_cast<GLfloat>(diffuseB->value()));

        m_CustomMaterial.specular.SetRed(static_cast<GLfloat>(specularR->value()));
        m_CustomMaterial.specular.SetGreen(static_cast<GLfloat>(specularG->value()));
        m_CustomMaterial.specular.SetBlue(static_cast<GLfloat>(specularB->value()));

        m_CustomMaterial.shininess = static_cast<GLfloat>(shininess->value());
        m_CurrentMaterial = &m_CustomMaterial;
      }
    );

    applySelectedMaterial(comboBox->currentIndex());
  }

  QSize size = sizeHint();
  setFixedSize(size.width() + 200, size.height() + 100);
}

TesterWidget::~TesterWidget() {
  m_PaintWidget->RunWithGLContext([this] {
    m_MeshCollection.Delete(m_PaintWidget);
  });
}
