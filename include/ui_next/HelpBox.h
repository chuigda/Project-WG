#ifndef PROJECT_WG_UINEXT_HELPBOX_H
#define PROJECT_WG_UINEXT_HELPBOX_H

#include "CloseSignallingWidget.h"

class HelpBox final : public CloseSignallingWidget {
public:
  explicit HelpBox(QWidget *parent = nullptr);
};

#endif // PROJECT_WG_UINEXT_HELPBOX_H
