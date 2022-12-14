#include "TesterWidget.h"

#include <vector>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include "cwglx/MeshLoader.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-make-unique"
TestMesh::TestMesh() {
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
    m_CurrentMesh(m_MeshCollection.monitorMesh.get()),
    m_CurrentMaterial(cw::GetBrassMaterial()),
    m_PaintWidget(new PaintWidget(&m_EntityStatus, &m_CurrentMesh, &m_CurrentMaterial))
{
  m_EntityStatus.translateY = 0;

  setWindowTitle("PHONG 光照模型调试器");

  QVBoxLayout *layout = new QVBoxLayout();
  setLayout(layout);
  layout->addWidget(m_PaintWidget);

  {
    QHBoxLayout *meshChooseLayout = new QHBoxLayout();
    layout->addLayout(meshChooseLayout);

    QLabel *meshChooseLabel = new QLabel("选择模型");
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("头部");
    comboBox->addItem("胸部");
    comboBox->addItem("大臂");
    comboBox->addItem("小臂");
    comboBox->addItem("手部");

    connect(comboBox, &QComboBox::currentIndexChanged, this, [this](int index) {
      switch (index) {
        case 0: m_CurrentMesh = m_MeshCollection.headMesh.get(); break;
        case 1: m_CurrentMesh = m_MeshCollection.chestMesh.get(); break;
        case 2: m_CurrentMesh = m_MeshCollection.bigArmFullMesh.get(); break;
        case 3: m_CurrentMesh = m_MeshCollection.smallArmFullMesh.get(); break;
        case 4: m_CurrentMesh = m_MeshCollection.clawFullMesh.get(); break;

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

    auto applySelectedMaterial = [this] (int index) {

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
  }
}

TesterWidget::~TesterWidget() {
  m_PaintWidget->RunWithGLContext([this] {
    m_MeshCollection.Delete(m_PaintWidget);
  });
}
