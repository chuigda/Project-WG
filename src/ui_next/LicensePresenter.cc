#include <QFile>
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QPlainTextEdit>
#include "ui_next/LicensePresenter.h"

LicenseContent::LicenseContent(QString const& fileName,
                               QString const& licenseName,
                               QString const& licenseBrief,
                               QString const& helpLink)
  : licenseName(licenseName.trimmed()),
    licenseBrief(licenseBrief.trimmed()),
    helpLink(helpLink.trimmed())
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
  : QDialog(parent),
    m_CurrentLicense(0)
{
  setWindowTitle("阅读协议");
  setFixedSize(600, 800);

  m_LicenseNameLabel = new QLabel("LicenseNamePlaceholder (?/?)");
  m_PrevLicenseButton = new QPushButton("◀");
  m_NextLicenseButton = new QPushButton("▶");

  QHBoxLayout *firstRow = new QHBoxLayout();
  firstRow->addWidget(m_LicenseNameLabel);
  firstRow->addStretch();
  firstRow->addWidget(m_PrevLicenseButton);
  firstRow->addWidget(m_NextLicenseButton);

  QLabel *notLegislative = new QLabel("<b>警告:</b> 此处文本不作为法律参考");
  m_BriefText = new QPlainTextEdit("");
  m_BriefText->setReadOnly(true);
  m_BriefText->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  m_BriefText->setMaximumHeight(240);

  QVBoxLayout *briefColumn = new QVBoxLayout();
  briefColumn->addWidget(notLegislative);
  briefColumn->addWidget(m_BriefText);
  m_BriefGroupBox = new QGroupBox("简介");
  m_BriefGroupBox->setLayout(briefColumn);

  m_LicenseText = new QPlainTextEdit("");
  m_LicenseText->setReadOnly(true);
  m_LicenseText->setFocusPolicy(Qt::FocusPolicy::NoFocus);

  QVBoxLayout *singleItemLayout = new QVBoxLayout();
  singleItemLayout->addWidget(m_LicenseText);
  QGroupBox *textGroupBox = new QGroupBox("正文");
  textGroupBox->setLayout(singleItemLayout);

  m_LicenseLink = new QLabel("LicenseLinkPlaceholder");

  m_DisagreeButton = new QPushButton("不同意");
  m_AgreeButton = new QPushButton("同意");
  QHBoxLayout *lastRow = new QHBoxLayout();
  lastRow->addWidget(m_DisagreeButton);
  lastRow->addStretch();
  lastRow->addWidget(m_AgreeButton);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->addLayout(firstRow);
  mainLayout->addWidget(m_BriefGroupBox);
  mainLayout->addWidget(textGroupBox);
  mainLayout->addWidget(m_LicenseLink);
  mainLayout->addLayout(lastRow);
  this->setLayout(mainLayout);

  connect(m_PrevLicenseButton, &QPushButton::clicked,
          this, &LicensePresenter::PrevLicense);
  connect(m_NextLicenseButton, &QPushButton::clicked,
          this, &LicensePresenter::NextLicense);
  connect(m_DisagreeButton, &QPushButton::clicked,
          this, &QDialog::reject);
  connect(m_AgreeButton, &QPushButton::clicked,
          this, &QDialog::accept);
}

void LicensePresenter::AddLicense(LicenseContent const& licenseContent) {
  m_Licenses.push_back(licenseContent);
}

void LicensePresenter::DisplayCurrentLicense() {
  LicenseContent const& licenseContent = m_Licenses.at(m_CurrentLicense);
  m_LicenseNameLabel->setText(
    QString("正在阅读: %1 (%2/%3)")
      .arg(licenseContent.licenseName)
      .arg(m_CurrentLicense + 1)
      .arg(m_Licenses.length())
  );

  m_PrevLicenseButton->setEnabled(m_CurrentLicense != 0);
  m_NextLicenseButton->setEnabled(m_CurrentLicense + 1 != m_Licenses.length());

  if (licenseContent.licenseBrief.isEmpty()) {
    m_BriefGroupBox->setVisible(false);
  } else {
    m_BriefGroupBox->setVisible(true);
    m_BriefText->setPlainText(licenseContent.licenseBrief);
  }

  m_LicenseText->setPlainText(licenseContent.licenseText);

  if (licenseContent.helpLink.isEmpty()) {
    m_LicenseLink->setVisible(false);
  } else {
    m_LicenseLink->setVisible(true);
    m_LicenseLink->setText(
      QString("<a href=\"%1\"> 更多信息请查阅 %1 </a>")
        .arg(licenseContent.helpLink)
    );
  }

  m_AgreeButton->setEnabled(m_CurrentLicense + 1 == m_Licenses.length());
}

void LicensePresenter::showEvent(QShowEvent *) {
  if (m_Licenses.isEmpty()) {
    this->close();
  }

  DisplayCurrentLicense();
}

void LicensePresenter::PrevLicense() {
  if (m_CurrentLicense == 0) {
    return;
  }

  m_CurrentLicense -= 1;
  DisplayCurrentLicense();
}

void LicensePresenter::NextLicense() {
  if (m_CurrentLicense + 1 == m_Licenses.length()) {
    return;
  }

  m_CurrentLicense += 1;
  DisplayCurrentLicense();
}
