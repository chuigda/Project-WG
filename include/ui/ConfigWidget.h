#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>

#include "cwglx/GLInfo.h"

class QListWidgetItem;
class CameraEntityStatus;
class ScreenStatus;

namespace Ui {
class ConfigWidget;
} // namespace Ui

class ConfigWidget final : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigWidget(CameraEntityStatus *cameraEntityStatus,
                        ScreenStatus *screenStatus,
                        QWidget *glWidget,
                        QWidget *parent = nullptr);
  ~ConfigWidget() final;

  void FillGLInfo(const cw::GLInfo &info);

public slots:
  void OnStaticScreensLoaded(QList<QListWidgetItem*> *staticScreens);
  void OnStaticScreenChosen(QListWidgetItem *item);
  void OnScreenAnimationReset();

private slots:
  void ReOpenWidget();

  void updateCameraX(int value);
  void updateCameraY(int value);
  void updateCameraZ(int value);
  void updateCameraRotationX(int value);
  void updateCameraRotationY(int value);
  void updateCameraRotationZ(int value);

  void updateEntityX(int value);
  void updateEntityY(int value);
  void updateEntityZ(int value);
  void updateEntityRotationX(int value);
  void updateEntityRotationY(int value);
  void updateEntityRotationZ(int value);

  void resetAll();

private:
  Ui::ConfigWidget *ui;

  QWidget *m_GLWidget;
  CameraEntityStatus *m_CameraEntityStatus;
  ScreenStatus *m_ScreenStatus;
};

#endif // CONFIGWIDGET_H
