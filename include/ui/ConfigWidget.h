#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>
#include "cwglx/GLInfo.h"

class CameraEntityStatus;

namespace Ui {
class ConfigWidget;
} // namespace Ui

class ConfigWidget final : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigWidget(CameraEntityStatus *cameraEntityStatus,
                        QWidget *glWidget,
                        QWidget *parent = nullptr);
  ~ConfigWidget() final;

  void FillGLInfo(const cw::GLInfo &info);

private slots:
  void reOpenGLWidget();

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
};

#endif // CONFIGWIDGET_H
