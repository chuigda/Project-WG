#ifndef PROJECT_WG_UINEXT_CAMERA_CONTROL_H
#define PROJECT_WG_UINEXT_CAMERA_CONTROL_H

#include <QWidget>
#include "ui_next/CloseSignallingWidget.h"

class CameraEntityStatus;

class CameraControl : public CloseSignallingWidget {
  Q_OBJECT

public:
  explicit CameraControl(CameraEntityStatus *status) noexcept;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void ResetCameraEntityStatus();
#pragma clang diagnostic pop

private:
  // Status output
  CameraEntityStatus *m_CameraEntityStatus;
};

#endif // PROJECT_WG_UINEXT_CAMERA_CONTROL_H
