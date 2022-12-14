#ifndef PROJECT_WG_TESTER_WIDGET_H
#define PROJECT_WG_TESTER_WIDGET_H

#include "PaintWidget.h"
#include "cwglx/drawable/PlainTriangles.h"

struct TestMesh {
  std::unique_ptr<cw::PlainTriangles> monitorMesh;
  std::unique_ptr<cw::PlainTriangles> monitorIntakeMesh;

  std::unique_ptr<cw::PlainTriangles> chestBoxMesh;
  std::unique_ptr<cw::PlainTriangles> chestPlateMesh;

  std::unique_ptr<cw::PlainTriangles> bigArmMesh;
  std::unique_ptr<cw::PlainTriangles> bigArmCoverMesh;

  std::unique_ptr<cw::PlainTriangles> smallArmMesh;
  std::unique_ptr<cw::PlainTriangles> smallArmCoverMesh;

  std::unique_ptr<cw::PlainTriangles> clawMesh;
  std::unique_ptr<cw::PlainTriangles> clawCoverMesh;

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
  cw::Material const* m_CurrentMaterial;

  PaintWidget *m_PaintWidget;
};

#endif // PROJECT_WG_TESTER_WIDGET_H
