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
#include "ui/FaceTrackStatus.h"
#include "ui/ScreenStatus.h"
#include "ui/Animation.h"

#include "wgc0310/BodyStatus.h"
#include "wgc0310/Mesh.h"
#include "wgc0310/Screen.h"

class QTimer;
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
  void StaticScreensLoaded(std::vector<StaticScreen>*);
  void AnimationsLoaded(std::vector<Animation>*);
  void BodyAnimationsLoaded(
    std::vector<std::unique_ptr<wgc0310::BodyAnimation>>*
  );
#pragma clang diagnostic pop

public slots:
  void OnRenderSettingsSet(GLenum cullFaceMode, int targetFPS);

private slots:
  void RequestNextFrame();

private:
  void InitPoseEstimator();
  void LoadAndInitScreens();
  void LoadAnimations();
  void InitAnimations();
  void LoadBodyAnimations();

  void DrawArm(const wgc0310::ArmStatus& armStatus, GLfloat coeff);

private:
  QScopedPointer<cw::Light> m_Light;
  QScopedPointer<cw::Light> m_Light2;

  cw::DrawableArena m_Arena;

  std::unique_ptr<wgc0310::WGCMeshCollection> m_Mesh;
  cw::Drawable const* m_ScreenGlass;
  wgc0310::Screen const* m_Screen;

  wgc0310::BodyStatus m_BodyStatus;
  CameraEntityStatus m_CameraEntityStatus;
  FaceTrackStatus m_FaceTrackStatus;
  ScreenStatus m_ScreenStatus;
  ConfigWidget *m_ConfigWidget;

  std::vector<StaticScreen> m_StaticScreens;
  std::vector<Animation> m_Animations;
  QList<QString> m_BodyAnimationNames;
  std::vector<std::unique_ptr<wgc0310::BodyAnimation>> m_BodyAnimations;

  QTimer *m_Timer;
};

#endif // PROJECT_WG_GLWIDGET_H
