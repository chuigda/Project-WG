#ifndef PROJECT_WG_UINEXT_GLWINDOW_H
#define PROJECT_WG_UINEXT_GLWINDOW_H

#include <QOpenGLWidget>
#include "cwglx/GL/GL.h"
#include "wgc0310/BodyStatus.h"
#include "wgc0310/AttachmentStatus.h"
#include "wgc0310/Screen.h"
#include "wgc0310/Mesh.h"
#include "wgc0310/HeadStatus.h"
#include "wgc0310/ScreenAnimationStatus.h"
#include "wgc0310/Shader.h"
#include "ui_next/EntityStatus.h"
#include "ui_next/ExtraControl.h"
#include "util/CircularBuffer.h"

class GLWindow final : public QOpenGLWidget {
  Q_OBJECT

public:
  explicit GLWindow(EntityStatus const* entityStatus,
                    wgc0310::HeadStatus const* headStatus,
                    wgc0310::BodyStatus const* bodyStatus,
                    wgc0310::AttachmentStatus const* attachmentStatus,
                    wgc0310::ScreenAnimationStatus const *screenAnimationStatus,
                    cw::CircularBuffer<qreal, 160> *volumeLevels,
                    bool *volumeLevelsUpdated,
                    wgc0310::ScreenDisplayMode const *screenDisplayMode,
                    StatusExtra const* statusExtra);
  ~GLWindow() final;

  void RunWithGLContext(std::function<void(void)> const& f);
  void EnablePerformanceCounter();
  GLuint64 QueryPerformanceCounter();

  void ReloadModel();
  bool SetShader(std::unique_ptr<wgc0310::ShaderCollection> &&shader);

  // OpenGL function
  GLFunctions *GL;

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

private:
  // Internal status
  qreal m_DevicePixelRatio;

  // Input status
  EntityStatus const* m_EntityStatus;
  wgc0310::HeadStatus const* m_HeadStatus;
  wgc0310::BodyStatus const* m_BodyStatus;
  wgc0310::AttachmentStatus const* m_AttachmentStatus;
  wgc0310::ScreenAnimationStatus const *m_ScreenAnimationStatus;
  cw::CircularBuffer<qreal, 160> *m_VolumeLevels;
  bool *m_VolumeLevelsUpdated;
  wgc0310::ScreenDisplayMode const *m_ScreenDisplayMode;

  StatusExtra const* m_StatusExtra;

  cw::GLObjectContext m_GLObjectContext;
  std::unique_ptr<wgc0310::ShaderCollection> m_Shader;
  std::unique_ptr<wgc0310::Screen> m_Screen;
  std::unique_ptr<wgc0310::WGCModel> m_Model;

  glm::mat4 m_Projection;

  GLuint m_PerformanceCounter;
  bool m_PerformanceCounterEnabled;
};

#endif // PROJECT_WG_UINEXT_GLWINDOW_H
