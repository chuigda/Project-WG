#ifndef PROJECT_WG_GLWIDGET_H
#define PROJECT_WG_GLWIDGET_H

#include <QOpenGLWidget>
#include <QScopedPointer>

#include "cwglx/GLImpl.h"
#include "cwglx/Light.h"
#include "cwglx/Texture.h"
#include "cwglx/Drawable.h"
#include "cwglx/DrawableArena.h"
#include "ui/CameraEntityStatus.h"
#include "ui/ScreenStatus.h"

class QListWidgetItem;
class ConfigWidget;

class GLWidget final : public QOpenGLWidget, public GLFunctions {
  Q_OBJECT

public:
  explicit GLWidget(QWidget *parent = nullptr);
  ~GLWidget() final;

protected:
  void initializeGL() final;
  void paintGL() final;
  void resizeGL(int w, int h) final;

  void mouseDoubleClickEvent(QMouseEvent *event) final;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void StaticScreensLoaded(QList<QListWidgetItem*>*);
#pragma clang diagnostic pop

private slots:
  void UpdateRotation();

private:
  void LoadStaticScreens();

private:
  QScopedPointer<cw::Light> m_Light;
  QScopedPointer<cw::Light> m_Light2;

  cw::DrawableArena m_Arena;
  std::size_t m_HeadId;
  std::size_t m_ScreenGlassId;
  std::size_t m_ScreenId;

  CameraEntityStatus m_CameraEntityStatus;
  ScreenStatus m_ScreenStatus;
  ConfigWidget *m_ConfigWidget;

  std::vector<std::unique_ptr<cw::Texture2D>> m_StaticScreens;
  QList<QListWidgetItem*> m_StaticScreenItems;
};

#endif // PROJECT_WG_GLWIDGET_H
