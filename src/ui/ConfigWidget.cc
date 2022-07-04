#include "ui/ConfigWidget.h"
#include "ui_ConfigWidget.h"

#include <QFile>
#include <QMessageBox>
#include "ui/CameraEntityStatus.h"
#include "ui/ScreenStatus.h"
#include "ui/MessageBoxAlter.h"

ConfigWidget::ConfigWidget(CameraEntityStatus *cameraEntityStatus,
                           ScreenStatus *screenStatus,
                           QWidget *glWidget) :
  QWidget(glWidget, Qt::Window),
  ui(new Ui::ConfigWidget),
  m_GLWidget(glWidget),
  m_CameraEntityStatus(cameraEntityStatus),
  m_ScreenStatus(screenStatus)
{
  ui->setupUi(this);

  // load AGPL text from resource file
  QFile file(":/LICENSE");
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
  ui->cameraXSlider->setValue(m_CameraEntityStatus->cameraX / 5);
  ui->cameraYSlider->setValue(m_CameraEntityStatus->cameraY / 5);
  ui->cameraZSlider->setValue(m_CameraEntityStatus->cameraZ / 5);

  ui->cameraXAngleSlider->setValue(m_CameraEntityStatus->cameraRotateX/ 10);
  ui->cameraYAngleSlider->setValue(m_CameraEntityStatus->cameraRotateY/ 10);
  ui->cameraZAngleSlider->setValue(m_CameraEntityStatus->cameraRotateZ/ 10);

  ui->entityXSlider->setValue(m_CameraEntityStatus->entityX / 5);
  ui->entityYSlider->setValue(m_CameraEntityStatus->entityY / 5);
  ui->entityZSlider->setValue(m_CameraEntityStatus->entityZ / 5);

  ui->entityXAngleSlider->setValue(m_CameraEntityStatus->entityRotateX / 10);
  ui->entityYAngleSlider->setValue(m_CameraEntityStatus->entityRotateY / 10);
  ui->entityZAngleSlider->setValue(m_CameraEntityStatus->entityRotateZ / 10);
#pragma clang diagnostic pop
  connect(ui->staticAnimList, &QListWidget::itemDoubleClicked,
          this, &ConfigWidget::OnStaticScreenChosen);
  connect(ui->resetScrAnimButton, &QPushButton::clicked,
          this, &ConfigWidget::OnScreenAnimationReset);

  connect(ui->reOpenButton, &QPushButton::clicked,
          this, &ConfigWidget::ReOpenWidget);

  connect(ui->cameraXSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateCameraX);
  connect(ui->cameraYSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateCameraY);
  connect(ui->cameraZSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateCameraZ);

  connect(ui->cameraXAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateCameraRotationX);
  connect(ui->cameraYAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateCameraRotationY);
  connect(ui->cameraZAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateCameraRotationZ);

  connect(ui->entityXSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateEntityX);
  connect(ui->entityYSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateEntityY);
  connect(ui->entityZSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateEntityZ);

  connect(ui->entityXAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateEntityRotationX);
  connect(ui->entityYAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateEntityRotationY);
  connect(ui->entityZAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::UpdateEntityRotationZ);

  connect(ui->resetTransButton, &QPushButton::clicked,
          this, &ConfigWidget::ResetAll);

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

void
ConfigWidget::OnStaticScreensLoaded(QList<QListWidgetItem*> *staticScreens) {
  for (auto staticScreen : *staticScreens) {
    ui->staticAnimList->addItem(staticScreen);
  }
}

void ConfigWidget::ReOpenWidget() {
  m_GLWidget->show();
}

void ConfigWidget::OnStaticScreenChosen(QListWidgetItem *item) {
  StaticScreenItem *staticScreenItem = static_cast<StaticScreenItem*>(item);
  m_ScreenStatus->PlayStaticAnimation(staticScreenItem);
}

void ConfigWidget::OnScreenAnimationReset() {
  m_ScreenStatus->Reset();
  ui->staticAnimList->clearSelection();
  ui->dynamicAnimList->clearSelection();
}

void ConfigWidget::UpdateCameraX(int value) {
  m_CameraEntityStatus->cameraX = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateCameraY(int value) {
  m_CameraEntityStatus->cameraY = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateCameraZ(int value) {
  m_CameraEntityStatus->cameraZ = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateCameraRotationX(int value) {
  m_CameraEntityStatus->cameraRotateX = static_cast<GLdouble>(value * 10);
}

void ConfigWidget::UpdateCameraRotationY(int value) {
  m_CameraEntityStatus->cameraRotateY = static_cast<GLdouble>(value * 10);
}

void ConfigWidget::UpdateCameraRotationZ(int value) {
  m_CameraEntityStatus->cameraRotateZ = static_cast<GLdouble>(value * 10);
}

void ConfigWidget::UpdateEntityX(int value) {
  m_CameraEntityStatus->entityX = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateEntityY(int value) {
  m_CameraEntityStatus->entityY = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateEntityZ(int value) {
  m_CameraEntityStatus->entityZ = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateEntityRotationX(int value) {
  m_CameraEntityStatus->entityRotateX = static_cast<GLdouble>(value * 10);
}

void ConfigWidget::UpdateEntityRotationY(int value) {
  m_CameraEntityStatus->entityRotateY = static_cast<GLdouble>(value * 10);
}

void ConfigWidget::UpdateEntityRotationZ(int value) {
  m_CameraEntityStatus->entityRotateZ = static_cast<GLdouble>(value * 10);
}

void ConfigWidget::ResetAll() {
  ui->cameraXSlider->setValue(0);
  ui->cameraYSlider->setValue(0);
  ui->cameraZSlider->setValue(0);

  ui->cameraXAngleSlider->setValue(0);
  ui->cameraYAngleSlider->setValue(0);
  ui->cameraZAngleSlider->setValue(0);

  ui->entityXSlider->setValue(0);
  ui->entityYSlider->setValue(0);
  ui->entityZSlider->setValue(-30 / 5);

  ui->entityXAngleSlider->setValue(0);
  ui->entityYAngleSlider->setValue(0);
  ui->entityZAngleSlider->setValue(0);

  m_CameraEntityStatus->cameraX = 0.0f;
  m_CameraEntityStatus->cameraY = 0.0f;
  m_CameraEntityStatus->cameraZ = 0.0f;

  m_CameraEntityStatus->cameraRotateX = 0.0;
  m_CameraEntityStatus->cameraRotateY = 0.0;
  m_CameraEntityStatus->cameraRotateZ = 0.0;

  m_CameraEntityStatus->entityX = 0.0f;
  m_CameraEntityStatus->entityY = 0.0f;
  m_CameraEntityStatus->entityZ = -30.0f;

  m_CameraEntityStatus->entityRotateX = 0.0;
  m_CameraEntityStatus->entityRotateY = 0.0;
  m_CameraEntityStatus->entityRotateZ = 0.0;
}

void ConfigWidget::ShowAGPLLicense() {
  MessageBoxAlter::Show("AGPL v3 License",
                        m_AGPLText,
                        this);
}

void ConfigWidget::ShowCCLicense() {
  MessageBoxAlter::Show("CC License",
                        m_CCText,
                        this);
}

void ConfigWidget::ShowAboutQt() {
  QMessageBox::aboutQt(this);
}