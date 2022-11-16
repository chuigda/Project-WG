#include "ui/ConfigWidget.h"
#include "ui/CameraEntityStatus.h"
#include "ui_ConfigWidget.h"

void ConfigWidget::SetupCameraEntityPage() {
  #pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
  ui->cameraXSlider->setValue(m_CameraEntityStatus->cameraX / 5);
  ui->cameraYSlider->setValue(m_CameraEntityStatus->cameraY / 5);
  ui->cameraZSlider->setValue(m_CameraEntityStatus->cameraZ / 5);

  ui->cameraXAngleSlider->setValue(m_CameraEntityStatus->cameraRotateX / 10);
  ui->cameraYAngleSlider->setValue(m_CameraEntityStatus->cameraRotateY / 10);
  ui->cameraZAngleSlider->setValue(m_CameraEntityStatus->cameraRotateZ / 10);

  ui->entityXSlider->setValue(m_CameraEntityStatus->entityX / 5);
  ui->entityYSlider->setValue(m_CameraEntityStatus->entityY / 5);
  ui->entityZSlider->setValue(m_CameraEntityStatus->entityZ / 5);

  ui->entityXAngleSlider->setValue(m_CameraEntityStatus->entityRotateX / 10);
  ui->entityYAngleSlider->setValue(m_CameraEntityStatus->entityRotateY / 10);
  ui->entityZAngleSlider->setValue(m_CameraEntityStatus->entityRotateZ / 10);
#pragma clang diagnostic pop

  connect(ui->confirmRenderSettingsButton, &QPushButton::clicked,
          this, &ConfigWidget::OnRenderSettingsConfirmed);
  connect(ui->resetRenderSettingsButton, &QPushButton::clicked,
          this, &ConfigWidget::OnRenderSettingsReset);

  connect(ui->resetScrAnimButton, &QPushButton::clicked,
          this, &ConfigWidget::OnScreenAnimationReset);

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
          this, &ConfigWidget::ResetCameraEntityAll);
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

void ConfigWidget::ResetCameraEntityAll() {
  ui->cameraXSlider->setValue(0);
  ui->cameraYSlider->setValue(0);
  ui->cameraZSlider->setValue(0);

  ui->cameraXAngleSlider->setValue(0);
  ui->cameraYAngleSlider->setValue(0);
  ui->cameraZAngleSlider->setValue(0);

  ui->entityXSlider->setValue(0);
  ui->entityYSlider->setValue(-25 / 5);
  ui->entityZSlider->setValue(-75 / 5);

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
  m_CameraEntityStatus->entityY = -25.0f;
  m_CameraEntityStatus->entityZ = -75.0f;

  m_CameraEntityStatus->entityRotateX = 0.0;
  m_CameraEntityStatus->entityRotateY = 0.0;
  m_CameraEntityStatus->entityRotateZ = 0.0;
}
