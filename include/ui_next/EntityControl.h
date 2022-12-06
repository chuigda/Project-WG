#ifndef PROJECT_WG_UINEXT_CAMERA_CONTROL_H
#define PROJECT_WG_UINEXT_CAMERA_CONTROL_H

#include "ui_next/CloseSignallingWidget.h"

class EntityStatus;

class EntityControl : public CloseSignallingWidget {
  Q_OBJECT

public:
  explicit EntityControl(EntityStatus *status) noexcept;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void ResetCameraEntityStatus();
#pragma clang diagnostic pop

private:
  // Status output
  EntityStatus *m_EntityStatus;
};

#endif // PROJECT_WG_UINEXT_CAMERA_CONTROL_H
