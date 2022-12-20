#ifndef PROJECT_WG_PAINTWIDGET_H
#define PROJECT_WG_PAINTWIDGET_H

#include <QOpenGLWidget>
#include "cwglx/GL.h"
#include "cwglx/GLImpl.h"
#include "wgc0310/Mesh.h"
#include "cwglx/Light.h"
#include "ui_next/EntityStatus.h"

class PaintWidget final : public QOpenGLWidget, public GLFunctions {
  Q_OBJECT

public:
  explicit PaintWidget(EntityStatus *entityStatus,
                       cw::Drawable **chosenMesh,
                       cw::StandardMaterial const**chosenMaterial);

  void RunWithGLContext(std::function<void(void)> const& f);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void OpenGLInitialized();
#pragma clang diagnostic pop

protected:
  void initializeGL() final;
  void paintGL() final;
  void resizeGL(int w, int h) final;

private:
  EntityStatus *m_EntityStatus;
  cw::Drawable* *m_ChosenMesh;
  cw::StandardMaterial const* *m_ChosenMaterial;

  std::unique_ptr<cw::Light> m_Light;
  std::unique_ptr<cw::Light> m_Light2;

  qreal m_DevicePixelRatio;
};

#endif // PROJECT_WG_PAINTWIDGET_H
