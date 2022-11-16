#include "ui/ConfigWidget.h"
#include "ui_ConfigWidget.h"

#include <QFile>
#include <QMessageBox>
#include "ui/CameraEntityStatus.h"
#include "include/wgc0310/BodyStatus.h"
#include "ui/ScreenStatus.h"
#include "ui/MessageBoxAlter.h"

ConfigWidget::ConfigWidget(CameraEntityStatus *cameraEntityStatus,
                           wgc0310::BodyStatus *bodyStatus,
                           ScreenStatus *screenStatus,
                           QWidget *glWidget) :
  QWidget(glWidget, Qt::Window),
  ui(new Ui::ConfigWidget),
  m_GLWidget(glWidget),
  m_CameraEntityStatus(cameraEntityStatus),
  m_BodyStatus(bodyStatus),
  m_ScreenStatus(screenStatus)
{
  ui->setupUi(this);

  // load third parties license text from resource file
  QFile file(":/3RD_PARTIES");
  if (file.open(QIODevice::ReadOnly)) {
    m_3rdPartiesText = file.readAll();
    file.close();
  } else {
    qCritical() << "could not open 3RD_PARTIES resource file";
    std::abort();
  }

  // load AGPL text from resource file
  file.setFileName(":/LICENSE");
  if (file.open(QIODevice::ReadOnly)) {
    m_AGPLText = file.readAll();
    file.close();
  } else {
    qCritical() << "Failed to open AGPL text file";
    std::abort();
  }

  // load CC text from resource file
  file.setFileName(":/LICENSE-CC-BY-NC-SA");
  if (file.open(QIODevice::ReadOnly)) {
    m_CCText = file.readAll();
    file.close();
  } else {
    qCritical() << "Failed to open CC-BY-NC-SA text file";
    std::abort();
  }

  SetupCameraEntityPage();
  SetupBodyPage();

  connect(ui->thridPartiesButton, &QPushButton::clicked,
          this, &ConfigWidget::ShowThirdParties);
  connect(ui->viewAGPLLicenseButton, &QPushButton::clicked,
          this, &ConfigWidget::ShowAGPLLicense);
  connect(ui->viewCCLicenseButton, &QPushButton::clicked,
          this, &ConfigWidget::ShowCCLicense);
  connect(ui->aboutQtButton, &QPushButton::clicked,
          this, &ConfigWidget::ShowAboutQt);
}

ConfigWidget::~ConfigWidget() {
  delete ui;
}

void ConfigWidget::FillGLInfo(const cw::GLInfo &info) {
  ui->glVendorText->setText(info.vendor);
  ui->glVersionText->setText(info.version);
  ui->glRendererText->setText(info.renderer);
  ui->glExtensionsText->setPlainText(info.extensions);
}

void ConfigWidget::OnRenderSettingsConfirmed() {
  GLenum cullFaceMode;
  if (ui->radioNoCull->isChecked()) {
    cullFaceMode = GL_NONE;
  } else if (ui->radioCullCCW->isChecked()) {
    cullFaceMode = GL_CCW;
  } else /* if (ui->radioCullCW->isChecked()) */ {
    cullFaceMode = GL_CW;
  }

  int targetFPS = ui->targetFPS->value();

  emit SetRenderSettings(cullFaceMode, targetFPS);
}

void ConfigWidget::OnRenderSettingsReset() {
  ui->radioCullCCW->setChecked(true);
  ui->targetFPS->setValue(90);

  emit SetRenderSettings(GL_CCW, 90);
}

void
ConfigWidget::OnStaticScreensLoaded(QList<QListWidgetItem*> *staticScreens) {
  for (auto staticScreen : *staticScreens) {
    ui->staticAnimList->addItem(staticScreen);
  }
}

void ConfigWidget::OnScreenAnimationsLoaded(QList<QListWidgetItem*> *animations) {
  for (auto animation : *animations) {
    ui->dynamicAnimList->addItem(animation);
  }
}

void ConfigWidget::OnStaticScreenChosen(QListWidgetItem *item) {
  StaticScreenItem *staticScreenItem = static_cast<StaticScreenItem*>(item);
  m_ScreenStatus->PlayStaticAnimation(staticScreenItem);
}

void ConfigWidget::OnAnimationChosen(QListWidgetItem *item) {
  AnimationItem *animationItem = static_cast<AnimationItem*>(item);
  m_ScreenStatus->PlayAnimation(animationItem);
}

void ConfigWidget::OnScreenAnimationReset() {
  m_ScreenStatus->Reset();
  ui->staticAnimList->clearSelection();
  ui->dynamicAnimList->clearSelection();
}

void ConfigWidget::ShowThirdParties() {
  MessageBoxAlter::Show("第三方许可", m_3rdPartiesText, this);
}

void ConfigWidget::ShowAGPLLicense() {
  MessageBoxAlter::Show("AGPL v3 License", m_AGPLText, this);
}

void ConfigWidget::ShowCCLicense() {
  MessageBoxAlter::Show("CC License", m_CCText, this);
}

void ConfigWidget::ShowAboutQt() {
  QMessageBox::aboutQt(this);
}
