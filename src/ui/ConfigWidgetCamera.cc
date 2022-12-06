#include "ui/ConfigWidget.h"
#include "ui/EntityStatus.h"
#include "ui_ConfigWidget.h"

void ConfigWidget::SetupCameraEntityPage() {
  ui->cameraXSlider->setValue(static_cast<int>(m_CameraEntityStatus->translateX / 5.0f));
  ui->cameraYSlider->setValue(static_cast<int>(m_CameraEntityStatus->translateX / 5.0f));
  ui->cameraZSlider->setValue(static_cast<int>(m_CameraEntityStatus->translateX / 5.0f));

  ui->entityXAngleSlider->setValue(static_cast<int>(m_CameraEntityStatus->entityRotateX / 10.0f));
  ui->entityYAngleSlider->setValue(static_cast<int>(m_CameraEntityStatus->entityRotateY / 10.0f));
  ui->entityZAngleSlider->setValue(static_cast<int>(m_CameraEntityStatus->entityRotateZ / 10.0f));

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
  m_CameraEntityStatus->translateX = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateCameraY(int value) {
  m_CameraEntityStatus->translateY = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateCameraZ(int value) {
  m_CameraEntityStatus->translateZ = static_cast<GLfloat>(value * 5);
}

void ConfigWidget::UpdateEntityRotationX(int value) {
  m_CameraEntityStatus->entityRotateX = static_cast<GLfloat>(value * 10);
}

void ConfigWidget::UpdateEntityRotationY(int value) {
  m_CameraEntityStatus->entityRotateY = static_cast<GLfloat>(value * 10);
}

void ConfigWidget::UpdateEntityRotationZ(int value) {
  m_CameraEntityStatus->entityRotateZ = static_cast<GLfloat>(value * 10);
}

void ConfigWidget::ResetCameraEntityAll() {
  m_CameraEntityStatus->Reset();

  ui->cameraXSlider->setValue(static_cast<int>(m_CameraEntityStatus->translateX / 5.0f));
  ui->cameraYSlider->setValue(static_cast<int>(m_CameraEntityStatus->translateX / 5.0f));
  ui->cameraZSlider->setValue(static_cast<int>(m_CameraEntityStatus->translateX / 5.0f));

  ui->entityXAngleSlider->setValue(static_cast<int>(m_CameraEntityStatus->entityRotateX / 10.0f));
  ui->entityYAngleSlider->setValue(static_cast<int>(m_CameraEntityStatus->entityRotateY / 10.0f));
  ui->entityZAngleSlider->setValue(static_cast<int>(m_CameraEntityStatus->entityRotateZ / 10.0f));
}
