#ifndef PROJECT_WG_UINEXT_GLWINDOW_H
#define PROJECT_WG_UINEXT_GLWINDOW_H

#include <QOpenGLWidget>
#include "cwglx/Drawable.h"
#include "cwglx/DrawableArena.h"
#include "cwglx/GL.h"
#include "cwglx/GLImpl.h"
#include "cwglx/Light.h"
#include "cwglx/Texture.h"
#include "wgc0310/BodyStatus.h"
#include "wgc0310/Mesh.h"
#include "wgc0310/Screen.h"
#include "wgc0310/HeadStatus.h"
#include "ui/CameraEntityStatus.h"
#include "util/CircularBuffer.h"

class GLWindow final : public QOpenGLWidget, public GLFunctions {
  Q_OBJECT

public:
  explicit GLWindow(CameraEntityStatus const* cameraEntityStatus,
                    wgc0310::HeadStatus const* headStatus,
                    cw::CircularBuffer<qreal, 160> *volumeLevels,
                    bool *volumeLevelsUpdated,
                    wgc0310::ScreenDisplayMode const *screenDisplayMode);
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
  wgc0310::HeadStatus const* m_HeadStatus;
  cw::CircularBuffer<qreal, 160> *m_VolumeLevels;
  bool *m_VolumeLevelsUpdated;
  wgc0310::ScreenDisplayMode const *m_ScreenDisplayMode;

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
  std::array<GLuint, 640> m_VolumeIndices;
  std::array<GLuint, 2> m_VolumeVBO;

  // Event-based timer
  QTimer *m_Timer;
};

#endif // PROJECT_WG_UINEXT_GLWINDOW_H
