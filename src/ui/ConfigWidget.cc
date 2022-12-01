#include "ui/ConfigWidget.h"
#include "ui_ConfigWidget.h"

#include <QFile>
#include <QMessageBox>
#include "ui/CameraEntityStatus.h"
#include "include/wgc0310/BodyStatus.h"
#include "include/wgc0310/ScreenAnimationStatus.h"
#include "ui/MessageBoxAlter.h"

ConfigWidget::ConfigWidget(CameraEntityStatus *cameraEntityStatus,
                           wgc0310::BodyStatus *bodyStatus,
                           FaceTrackStatus *faceTrackStatus,
                           wgc0310::ScreenAnimationStatus *screenStatus,
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
  SetupCapturePage(faceTrackStatus);

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
ConfigWidget::OnStaticScreensLoaded(std::vector<wgc0310::StaticScreenImage> *staticScreens) {
  QVBoxLayout *layout = new QVBoxLayout();
  ui->staticGraphBox->setLayout(layout);

  for (auto &staticScreen : *staticScreens) {
    QPushButton *button = new QPushButton(staticScreen.imageName);
    layout->addWidget(button);

    wgc0310::StaticScreenImage *ptr = &staticScreen;
    connect(button, &QPushButton::clicked, [this, ptr] {
      m_ScreenStatus->PlayStaticAnimation(ptr);
    });
  }

  layout->addStretch();
}

void ConfigWidget::OnScreenAnimationsLoaded(
  std::vector<std::unique_ptr<wgc0310::ScreenAnimation>> *animations
) {
  QVBoxLayout *layout = new QVBoxLayout();
  ui->dynamicAnimationBox->setLayout(layout);

  for (auto &animation : *animations) {
    QPushButton *button = new QPushButton(animation->rawAnimation->name);
    layout->addWidget(button);

    wgc0310::ScreenAnimation *ptr = animation.get();
    connect(button, &QPushButton::clicked, [this, ptr] {
      m_ScreenStatus->PlayAnimation(ptr);
    });
  }

  layout->addStretch();
}

void ConfigWidget::OnScreenAnimationReset() {
  m_ScreenStatus->Reset();
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
