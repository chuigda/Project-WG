#ifndef PROJECT_WG_UINEXT_LICENSE_PRESENTER_H
#define PROJECT_WG_UINEXT_LICENSE_PRESENTER_H

#include <QDialog>
#include "util/Derive.h"

class QLabel;
class QPushButton;
class QPlainTextEdit;
class QGroupBox;

struct LicenseContent {
  LicenseContent(QString const& licenseName,
                 QString licenseText,
                 QString const& licenseBrief = "",
                 QString const& helpLink = "");

  CW_DERIVE_DEFAULT_COPY(LicenseContent)

  QString licenseName;
  QString licenseBrief;
  QString licenseText;
  QString helpLink;
};

class LicensePresenter final : public QDialog {
  Q_OBJECT

public:
  explicit LicensePresenter(QWidget *parent = nullptr);

  void AddLicense(LicenseContent const& licenseContent);

  void DisplayCurrentLicense();

  void RequireAgreement(bool required = true);

  void showEvent(QShowEvent *) final;

private slots:
  void PrevLicense();

  void NextLicense();

private:
  QList<LicenseContent> m_Licenses;
  qsizetype m_CurrentLicense;

  QLabel *m_LicenseNameLabel;
  QPushButton *m_PrevLicenseButton;
  QPushButton *m_NextLicenseButton;
  QGroupBox *m_BriefGroupBox;
  QPlainTextEdit *m_BriefText;
  QPlainTextEdit *m_LicenseText;
  QLabel *m_LicenseLink;
  QPushButton *m_OkButton;
};

#endif
