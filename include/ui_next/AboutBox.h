#ifndef PROJECT_WG_UINEXT_ABOUT_BOX_H
#define PROJECT_WG_UINEXT_ABOUT_BOX_H

#include "ui_next/CloseSignallingWidget.h"
#include "LicensePresenter.h"

class AboutBox final: public CloseSignallingWidget {
  Q_OBJECT

public:
  explicit AboutBox(LicensePresenter *presenter);
  ~AboutBox() noexcept final;

private:
  LicensePresenter *m_LicensePresenter;
};

#endif // PROJECT_WG_UINEXT_ABOUT_BOX_H
