#ifndef PROJECT_WG_UINEXT_GLWINDOW_H
#define PROJECT_WG_UINEXT_GLWINDOW_H

#include <QOpenGLWidget>
#include "cwglx/GL.h"
#include "cwglx/GLImpl.h"
#include "cwglx/Light.h"
#include "cwglx/Drawable.h"
#include "cwglx/DrawableArena.h"
#include "wgc0310/BodyStatus.h"
#include "wgc0310/Mesh.h"
#include "wgc0310/Screen.h"
#include "cwglx/Texture.h"
#include "ui/CameraEntityStatus.h"

class GLWindow final : public QOpenGLWidget, public GLFunctions {
  Q_OBJECT

public:
  GLWindow(CameraEntityStatus const* cameraEntityStatus);
  ~GLWindow() final;

  void RunWithGLContext(std::function<void(void)> const& f);

protected:
  void initializeGL() final;
  void paintGL() final;
  void resizeGL(int w, int h) final;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void OpenGLInitialized();
#pragma clang diagnostic pop

private:
  // Input status
  CameraEntityStatus const* m_CameraEntityStatus;

  // Internal states, OpenGL resources and so on
  std::unique_ptr<cw::Light> m_Light;
  std::unique_ptr<cw::Light> m_Light2;

  cw::DrawableArena m_Arena;

  wgc0310::WGCMeshCollection m_Mesh;
  cw::Drawable const* m_ScreenGlass;
  wgc0310::Screen const* m_Screen;

  std::unique_ptr<cw::Texture2D> m_EyeTexture;
  std::unique_ptr<cw::Texture2D> m_MouthTexture;
  std::unique_ptr<cw::Texture2D> m_MouthOpenTexture;
  std::vector<GLuint> m_VolumeIndices;
  std::array<GLuint, 2> m_VolumeVBO;

  QTimer *m_Timer;
};

#endif // PROJECT_WG_UINEXT_GLWINDOW_H
