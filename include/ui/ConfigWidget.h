#ifndef PROJECT_WG_CONFIG_WIDGET_H
#define PROJECT_WG_CONFIG_WIDGET_H

#include <QWidget>
#include <QtGui/qopengl.h>

#include "cwglx/GLInfo.h"
#include "wgc0310/BodyAnim.h"
#include "cwglx/Texture.h"
#include "ui/FaceTrackStatus.h"

class EntityStatus;

namespace wgc0310 {
struct StaticScreenImage;
class ScreenAnimation;
class ScreenAnimationStatus;
class BodyStatus;
} // namespace wgc0310;

namespace Ui {
class ConfigWidget;
} // namespace Ui

class ConfigWidget final : public QWidget {
  Q_OBJECT

public:
  explicit ConfigWidget(EntityStatus *cameraEntityStatus,
                        wgc0310::BodyStatus *bodyStatus,
                        FaceTrackStatus *faceTrackStatus,
                        wgc0310::ScreenAnimationStatus *screenStatus,
                        QWidget *glWidget);
  ~ConfigWidget() final;

  void FillGLInfo(const cw::GLInfo &info);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void SetRenderSettings(GLenum cullFaceMode, int targetFPS);
  void StartBodyAnimation();
  void DoneBodyAnimation();
#pragma clang diagnostic pop

public slots:
  void OnStaticScreensLoaded(
    std::vector<wgc0310::StaticScreenImage> *staticScreens
  );
  void OnScreenAnimationsLoaded(
    std::vector<std::unique_ptr<wgc0310::ScreenAnimation>> *animations
  );
  void OnBodyAnimationsLoaded(
    std::vector<std::unique_ptr<wgc0310::BodyAnimation>> *animations
  );

private slots:
  void OnRenderSettingsConfirmed();
  void OnRenderSettingsReset();

  void OnScreenAnimationReset();

  void UpdateCameraX(int value);
  void UpdateCameraY(int value);
  void UpdateCameraZ(int value);
  void UpdateEntityRotationX(int value);
  void UpdateEntityRotationY(int value);
  void UpdateEntityRotationZ(int value);
  void ResetCameraEntityAll();

  void ShowThirdParties();
  void ShowAGPLLicense();
  void ShowCCLicense();
  void ShowAboutQt();

private:
  void SetupCameraEntityPage();
  void SetupBodyPage();
  void SetupCapturePage(FaceTrackStatus *faceTrackStatus);

  Ui::ConfigWidget *ui;

  QWidget *m_GLWidget;
  EntityStatus *m_CameraEntityStatus;
  wgc0310::BodyStatus *m_BodyStatus;
  wgc0310::ScreenAnimationStatus *m_ScreenStatus;

  QString m_3rdPartiesText;
  QString m_AGPLText;
  QString m_CCText;
};

#endif // PROJECT_WG_CONFIG_WIDGET_H
