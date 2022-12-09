#ifndef PROJECT_WG_UINEXT_EXTRA_CONTROL_H
#define PROJECT_WG_UINEXT_EXTRA_CONTROL_H

#include "ui_next/CloseSignallingWidget.h"
#include "cwglx/Color.h"

struct StatusExtra {
  bool customClearColor = false;
  cw::RGBAColor clearColor { 0, 0, 0, 255 };

  bool stayOnTop = false;
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
