#ifndef PROJECT_WG_UINEXT_EXTRA_CONTROL_H
#define PROJECT_WG_UINEXT_EXTRA_CONTROL_H

#include "ui_next/CloseSignallingWidget.h"
#include "cwglx/Color.h"

struct StatusExtra {
  bool stayOnTop = false;

  bool customClearColor = false;
  cw::RGBAColorF clearColor { 0.0f, 0.0f, 0.0f, 1.0f };

  bool stroke = false;
  cw::RGBAColorF strokeColor { 0.0f, 0.0f, 0.0f, 1.0f };
  GLfloat strokeLineWidth = 1.0f;
  GLfloat strokeAdjustment = 1.001f;
};

class ExtraControl final : public CloseSignallingWidget {
  Q_OBJECT

public:
  explicit ExtraControl(StatusExtra *statusExtra);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void SetStayOnTop(bool stayOnTop);
#pragma clang diagnostic pop

private:
  StatusExtra *m_StatusExtra;
};

#endif // PROJECT_WG_UINEXT_EXTRA_CONTROL_H
