#ifndef PROJECT_WG_UINEXT_BODY_CONTROL_H
#define PROJECT_WG_UINEXT_BODY_CONTROL_H

#include "wgc0310/BodyStatus.h"
#include "ui_next/CloseSignallingWidget.h"

class ControlPanel;

class BodyControl : public CloseSignallingWidget {
  Q_OBJECT

public:
  BodyControl(wgc0310::BodyStatus *bodyStatus,
              ControlPanel *controlPanel);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void StartBodyAnimation();
#pragma clang diagnostic pop

private:
  wgc0310::BodyStatus *m_BodyStatus;
  std::vector<std::unique_ptr<wgc0310::BodyAnimation>> m_BodyAnimations;
};

#endif // PROJECT_WG_UINEXT_BODY_CONTROL_H
