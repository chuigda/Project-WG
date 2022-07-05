#ifndef PROJECT_WG_CONFIG_WIDGET_H
#define PROJECT_WG_CONFIG_WIDGET_H

#include <QWidget>

#include "cwglx/GLInfo.h"

class QListWidgetItem;
class CameraEntityStatus;
class ScreenStatus;

namespace Ui {
class ConfigWidget;
} // namespace Ui

class ConfigWidget final : public QWidget {
  Q_OBJECT

public:
  explicit ConfigWidget(CameraEntityStatus *cameraEntityStatus,
                        ScreenStatus *screenStatus,
                        QWidget *glWidget);
  ~ConfigWidget() final;

  void FillGLInfo(const cw::GLInfo &info);

public slots:
  void OnStaticScreensLoaded(QList<QListWidgetItem*> *staticScreens);
  void OnAnimationsLoaded(QList<QListWidgetItem*> *animations);

private slots:
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
  void ResetAll();

  void ShowThirdParties();
  void ShowAGPLLicense();
  void ShowCCLicense();
  void ShowAboutQt();

private:
  Ui::ConfigWidget *ui;

  QWidget *m_GLWidget;
  CameraEntityStatus *m_CameraEntityStatus;
  ScreenStatus *m_ScreenStatus;

  QString m_3rdPartiesText;
  QString m_AGPLText;
  QString m_CCText;
};

#endif // PROJECT_WG_CONFIG_WIDGET_H
