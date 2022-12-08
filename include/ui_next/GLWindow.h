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
#include "wgc0310/ScreenAnimationStatus.h"
#include "ui_next/EntityStatus.h"
#include "util/CircularBuffer.h"
#include "ExtraControl.h"

namespace cw {
struct RawMatrix;
} // namespace cw

class GLWindow final : public QOpenGLWidget, public GLFunctions {
  Q_OBJECT

public:
  explicit GLWindow(EntityStatus const* entityStatus,
                    wgc0310::HeadStatus const* headStatus,
                    wgc0310::BodyStatus const* bodyStatus,
                    wgc0310::ScreenAnimationStatus const *screenAnimationStatus,
                    cw::CircularBuffer<qreal, 160> *volumeLevels,
                    bool *volumeLevelsUpdated,
                    wgc0310::ScreenDisplayMode const *screenDisplayMode,
                    StatusExtra const* statusExtra);
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
  void DrawScreenContent();
  void DrawArm(const wgc0310::ArmStatus &armStatus,
               GLfloat coeff,
               cw::RawMatrix *bigArmMat,
               cw::RawMatrix *smallArmMat);

private:
  // Input status
  EntityStatus const* m_EntityStatus;
  wgc0310::HeadStatus const* m_HeadStatus;
  wgc0310::BodyStatus const* m_BodyStatus;
  wgc0310::ScreenAnimationStatus const *m_ScreenAnimationStatus;
  cw::CircularBuffer<qreal, 160> *m_VolumeLevels;
  bool *m_VolumeLevelsUpdated;
  wgc0310::ScreenDisplayMode const *m_ScreenDisplayMode;
  StatusExtra const* m_StatusExtra;

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
  std::array<cw::Vertex2DF, 640> m_VolumeVertices;
  std::array<GLuint, 640> m_VolumeIndices;
  std::array<GLuint, 2> m_VolumeVBO;
};

#endif // PROJECT_WG_UINEXT_GLWINDOW_H
