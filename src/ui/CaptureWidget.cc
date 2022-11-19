#include "ui/CaptureWidget.h"

#include <QPainter>
#include <QKeyEvent>

CaptureWidget::CaptureWidget(FaceTrackStatus *status, QWidget *parent)
  : QWidget(parent),
    m_Status(status)
{
  this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  this->setMinimumSize(300, 300);
  this->setMaximumSize(300, 300);
  this->setFixedWidth(300);
  this->setFixedHeight(300);

  this->setStyleSheet("border: 1px solid black;");
  this->setFocusPolicy(Qt::StrongFocus);
  this->setAttribute(Qt::WA_Hover, true);
}

void CaptureWidget::paintEvent(QPaintEvent*) {
  if (this->hasFocus()) {
    QPainter painter(this);
    if (m_Status->pose.mouthStatus == HeadPose::Open) {
      painter.setBrush(QColor(0, 0xcd, 0));
    } else {
      painter.setBrush(QColor(0xcd, 0, 0));
    }
    painter.drawEllipse(m_Status->pose.rotationZ * -10.0 + 145,
                        m_Status->pose.rotationX * -10.0 + 145,
                        10, 10);
  }
}

void CaptureWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Q:
      qDebug() << "open";
      m_Status->pose.mouthStatus =
        static_cast<HeadPose::MouthStatus>(-m_Status->pose.mouthStatus);
      update();
      return;
    case Qt::Key_Escape:
      clearFocus();
      return;
  }

  QWidget::keyPressEvent(event);
}

bool CaptureWidget::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::HoverMove:
      hoverMove(static_cast<QHoverEvent*>(event));
      return true;
    default:
      return QWidget::event(event);
  }
}

void CaptureWidget::hoverMove(QHoverEvent *event) {
  if (hasFocus()) {
    auto pos = event->oldPos();
    int dx = pos.x() - 150;
    int dy = pos.y() - 150;

    float yRotation = static_cast<float>(dx) / 15.0f;
    float zRotation = static_cast<float>(dx) / -10.0f;
    float xRotation = static_cast<float>(dy) / -10.0f;

    m_Status->pose.rotationY =
      m_Status->pose.rotationY * 0.5f + yRotation * 0.5f;
    m_Status->pose.rotationX =
      m_Status->pose.rotationX * 0.5f + xRotation * 0.5f;
    m_Status->pose.rotationZ =
      m_Status->pose.rotationZ * 0.5f + zRotation * 0.5f;

    update();
  }
}
