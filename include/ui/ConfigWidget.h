#ifndef PROJECT_WG_CONFIG_WIDGET_H
#define PROJECT_WG_CONFIG_WIDGET_H

#include <QWidget>
#include <QtGui/qopengl.h>

#include "cwglx/GLInfo.h"

class QListWidgetItem;
class CameraEntityStatus;
class ScreenStatus;

namespace wgc0310 {
class BodyStatus;
} // namespace wgc0310;

namespace Ui {
class ConfigWidget;
} // namespace Ui

class ConfigWidget final : public QWidget {
  Q_OBJECT

public:
  explicit ConfigWidget(CameraEntityStatus *cameraEntityStatus,
                        wgc0310::BodyStatus *bodyStatus,
                        ScreenStatus *screenStatus,
                        QWidget *glWidget);
  ~ConfigWidget() final;

  void FillGLInfo(const cw::GLInfo &info);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void SetRenderSettings(GLenum cullFaceMode, int targetFPS);
#pragma clang diagnostic pop

public slots:
  void OnStaticScreensLoaded(QList<QListWidgetItem*> *staticScreens);
  void OnAnimationsLoaded(QList<QListWidgetItem*> *animations);

private slots:
  void OnRenderSettingsConfirmed();
  void OnRenderSettingsReset();

  void OnStaticScreenChosen(QListWidgetItem *item);
  void OnAnimationChosen(QListWidgetItem *item);
  void OnScreenAnimationReset();

  void UpdateCameraX(int value);
  void UpdateCameraY(int value);
  void UpdateCameraZ(int value);
  void UpdateCameraRotationX(int value);
  void UpdateCameraRotationY(int value);
  void UpdateCameraRotationZ(int value);
  void UpdateEntityX(int value);
  void UpdateEntityY(int value);
  void UpdateEntityZ(int value);
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

  Ui::ConfigWidget *ui;

  QWidget *m_GLWidget;
  CameraEntityStatus *m_CameraEntityStatus;
  wgc0310::BodyStatus *m_BodyStatus;
  ScreenStatus *m_ScreenStatus;

  QString m_3rdPartiesText;
  QString m_AGPLText;
  QString m_CCText;
};

#endif // PROJECT_WG_CONFIG_WIDGET_H
