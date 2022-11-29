#ifndef PROJECT_WG_UINEXT_LICENSE_PRESENTER_H
#define PROJECT_WG_UINEXT_LICENSE_PRESENTER_H

#include <QWidget>

#include "util/Derive.h"

struct LicenseContent {
  LicenseContent(QString const& fileName,
                 QString  licenseName,
                 QString  licenseBrief = "");

  CW_DERIVE_DEFAULT_COPY(LicenseContent)

  QString licenseName;
  QString licenseBrief;
  QString licenseText;
};

class LicensePresenter : public QWidget {
  Q_OBJECT

public:
  explicit LicensePresenter(QWidget *parent = nullptr);

  void AddLicense(LicenseContent const& licenseContent);

private:
  QList<LicenseContent> m_Licenses;
  qsizetype m_CurrentLicense;
};

#endif
