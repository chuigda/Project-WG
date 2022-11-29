#include <QFile>
#include <utility>
#include "ui_next/LicensePresenter.h"

LicenseContent::LicenseContent(QString const& fileName,
                               QString licenseName,
                               QString licenseBrief)
  : licenseName(std::move(licenseName)),
    licenseBrief(std::move(licenseBrief))
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "cannot open license file:" << fileName;
    return;
  }

  QTextStream textStream(&file);
  licenseText = textStream.readAll();
}

LicensePresenter::LicensePresenter(QWidget *parent)
  : QWidget(parent, Qt::Dialog)
{
  setWindowTitle("阅读协议");
  setFixedSize(600, 800);
}

void LicensePresenter::AddLicense(LicenseContent const& licenseContent) {
  m_Licenses.push_back(licenseContent);
}
