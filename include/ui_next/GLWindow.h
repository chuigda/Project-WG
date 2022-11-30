#ifndef PROJECT_WG_UINEXT_GLWINDOW_H
#define PROJECT_WG_UINEXT_GLWINDOW_H

#include <QOpenGLWindow>
#include "cwglx/GL.h"
#include "cwglx/GLImpl.h"
#include "cwglx/Light.h"
#include "cwglx/Drawable.h"
#include "cwglx/DrawableArena.h"
#include "wgc0310/BodyStatus.h"
#include "wgc0310/Mesh.h"
#include "wgc0310/Screen.h"

class GLWindow final : public QOpenGLWindow, public GLFunctions {
  Q_OBJECT

public:
  GLWindow();
  ~GLWindow() final;

  void RunWithGLContext(std::function<void(void)> const& f);

protected:
  void initializeGL() final;
  void paintGL() final;
  void resizeGL(int w, int h) final;

private:
  std::unique_ptr<cw::Light> m_Light;
  std::unique_ptr<cw::Light> m_Light2;

  cw::DrawableArena m_Arena;

  std::unique_ptr<wgc0310::WGCMeshCollection> m_Mesh;
  cw::Drawable const* m_ScreenGlass;
  wgc0310::Screen const* m_Screen;
};

#endif // PROJECT_WG_UINEXT_GLWINDOW_H
