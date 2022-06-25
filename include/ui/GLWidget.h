#ifndef PROJECT_WG_GLWIDGET_H
#define PROJECT_WG_GLWIDGET_H

#include <QOpenGLWidget>
#include <QScopedPointer>

#include "cwglx/GLImpl.h"
#include "cwglx/Light.h"
#include "cwglx/Drawable.h"
#include "cwglx/DrawableArena.h"

class GLWidget final : public QOpenGLWidget, public GLFunctions {
  Q_OBJECT

public:
  explicit GLWidget(QWidget *parent = nullptr);
  ~GLWidget() final;

protected:
  void timerEvent(QTimerEvent *event) final;

  void initializeGL() final;
  void paintGL() final;
  void resizeGL(int w, int h) final;

private:
  QScopedPointer<cw::Light> m_Light;
  QScopedPointer<cw::Light> m_Light2;
  GLfloat m_Rotation;

  cw::DrawableArena m_Arena;
  std::size_t m_HeadId;
  std::size_t m_ScreenGlassId;
  std::size_t m_ScreenId;
};

#endif // PROJECT_WG_GLWIDGET_H
