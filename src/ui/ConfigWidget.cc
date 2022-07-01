#include "ui/ConfigWidget.h"
#include "ui/CameraEntityStatus.h"
#include "ui_ConfigWidget.h"

ConfigWidget::ConfigWidget(CameraEntityStatus *cameraEntityStatus,
                           QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ConfigWidget),
  m_CameraEntityStatus(cameraEntityStatus)
{
  ui->setupUi(this);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
  ui->cameraXSlider->setValue(m_CameraEntityStatus->cameraX);
  ui->cameraYSlider->setValue(m_CameraEntityStatus->cameraY);
  ui->cameraZSlider->setValue(m_CameraEntityStatus->cameraZ);

  ui->cameraXAngleSlider->setValue(m_CameraEntityStatus->cameraRotateX);
  ui->cameraYAngleSlider->setValue(m_CameraEntityStatus->cameraRotateY);
  ui->cameraZAngleSlider->setValue(m_CameraEntityStatus->cameraRotateZ);

  ui->entityXSlider->setValue(m_CameraEntityStatus->entityX);
  ui->entityYSlider->setValue(m_CameraEntityStatus->entityY);
  ui->entityZSlider->setValue(m_CameraEntityStatus->entityZ);

  ui->entityXAngleSlider->setValue(m_CameraEntityStatus->entityRotateX);
  ui->entityYAngleSlider->setValue(m_CameraEntityStatus->entityRotateY);
  ui->entityZAngleSlider->setValue(m_CameraEntityStatus->entityRotateZ);
#pragma clang diagnostic pop

  connect(ui->cameraXSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateCameraX);
  connect(ui->cameraYSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateCameraY);
  connect(ui->cameraZSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateCameraZ);

  connect(ui->cameraXAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateCameraRotationX);
  connect(ui->cameraYAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateCameraRotationY);
  connect(ui->cameraZAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateCameraRotationZ);

  connect(ui->entityXSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateEntityX);
  connect(ui->entityYSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateEntityY);
  connect(ui->entityZSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateEntityZ);

  connect(ui->entityXAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateEntityRotationX);
  connect(ui->entityYAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateEntityRotationY);
  connect(ui->entityZAngleSlider, &QSlider::valueChanged,
          this, &ConfigWidget::updateEntityRotationZ);

  connect(ui->resetTransButton, &QPushButton::clicked,
          this, &ConfigWidget::resetAll);
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

void ConfigWidget::updateCameraX(int value) {
  m_CameraEntityStatus->cameraX = static_cast<GLfloat>(value);
}

void ConfigWidget::updateCameraY(int value) {
  m_CameraEntityStatus->cameraY = static_cast<GLfloat>(value);
}

void ConfigWidget::updateCameraZ(int value) {
  m_CameraEntityStatus->cameraZ = static_cast<GLfloat>(value);
}

void ConfigWidget::updateCameraRotationX(int value) {
  m_CameraEntityStatus->cameraRotateX = static_cast<GLdouble>(value);
}

void ConfigWidget::updateCameraRotationY(int value) {
  m_CameraEntityStatus->cameraRotateY = static_cast<GLdouble>(value);
}

void ConfigWidget::updateCameraRotationZ(int value) {
  m_CameraEntityStatus->cameraRotateZ = static_cast<GLdouble>(value);
}


void ConfigWidget::updateEntityX(int value) {
  m_CameraEntityStatus->entityX = static_cast<GLfloat>(value);
}

void ConfigWidget::updateEntityY(int value) {
  m_CameraEntityStatus->entityY = static_cast<GLfloat>(value);
}

void ConfigWidget::updateEntityZ(int value) {
  m_CameraEntityStatus->entityZ = static_cast<GLfloat>(value);
}

void ConfigWidget::updateEntityRotationX(int value) {
  m_CameraEntityStatus->entityRotateX = static_cast<GLdouble>(value);
}

void ConfigWidget::updateEntityRotationY(int value) {
  m_CameraEntityStatus->entityRotateY = static_cast<GLdouble>(value);
}

void ConfigWidget::updateEntityRotationZ(int value) {
  m_CameraEntityStatus->entityRotateZ = static_cast<GLdouble>(value);
}

void ConfigWidget::resetAll() {
  ui->cameraXSlider->setValue(0);
  ui->cameraYSlider->setValue(0);
  ui->cameraZSlider->setValue(0);

  ui->cameraXAngleSlider->setValue(0);
  ui->cameraYAngleSlider->setValue(0);
  ui->cameraZAngleSlider->setValue(0);

  ui->entityXSlider->setValue(0);
  ui->entityYSlider->setValue(0);
  ui->entityZSlider->setValue(-30);

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
