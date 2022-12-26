#ifndef PROJECT_WG_TESTER_WIDGET_H
#define PROJECT_WG_TESTER_WIDGET_H

#include "PaintWidget.h"
#include "cwglx/drawable/PlainMesh.h"

struct TestMesh {
  std::unique_ptr<cw::PlainMesh> teapotMesh;

  std::unique_ptr<cw::PlainMesh> monitorMesh;
  std::unique_ptr<cw::PlainMesh> monitorIntakeMesh;

  std::unique_ptr<cw::PlainMesh> chestBoxMesh;
  std::unique_ptr<cw::PlainMesh> chestPlateMesh;

  std::unique_ptr<cw::PlainMesh> bigArmMesh;
  std::unique_ptr<cw::PlainMesh> bigArmCoverMesh;

  std::unique_ptr<cw::PlainMesh> smallArmMesh;
  std::unique_ptr<cw::PlainMesh> smallArmCoverMesh;

  std::unique_ptr<cw::PlainMesh> clawMesh;
  std::unique_ptr<cw::PlainMesh> clawCoverMesh;

  std::unique_ptr<cw::Drawable> headMesh;
  std::unique_ptr<cw::Drawable> chestMesh;
  std::unique_ptr<cw::Drawable> bigArmFullMesh;
  std::unique_ptr<cw::Drawable> smallArmFullMesh;
  std::unique_ptr<cw::Drawable> clawFullMesh;

  TestMesh();

  void Delete(GLFunctions *f);
};

class TesterWidget final : public QWidget {
  Q_OBJECT
public:
  TesterWidget();
  ~TesterWidget() final;

private:
  TestMesh m_MeshCollection;

  EntityStatus m_EntityStatus;
  cw::StandardMaterial m_CustomMaterial;
  cw::Drawable* m_CurrentMesh;
  cw::StandardMaterial const* m_CurrentMaterial;

  PaintWidget *m_PaintWidget;

  GLfloat rotation;
};

#endif // PROJECT_WG_TESTER_WIDGET_H
