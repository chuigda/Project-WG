#include "ui/ManualOperateWidget.h"

#include <QPainter>
#include <QKeyEvent>

ManualOperateWidget::ManualOperateWidget(FaceTrackStatus *status, QWidget *parent)
  : QWidget(parent),
    m_Status(status)
{
  this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  this->setMinimumSize(500, 500);
  this->setMaximumSize(500, 500);
  this->setFixedWidth(500);
  this->setFixedHeight(500);

  this->setStyleSheet("border: 1px solid black;");
  this->setFocusPolicy(Qt::StrongFocus);
  this->setAttribute(Qt::WA_Hover, true);
}

void ManualOperateWidget::paintEvent(QPaintEvent*) {
  if (this->hasFocus()) {
    QPainter painter(this);

    painter.setBrush(QColor(0xff, 0xff, 0xff));
    painter.drawRect(100, 100, 300, 300);

    painter.drawRect(245, 245, 10, 10);

    switch (m_Status->pose.mouthStatus) {
    case HeadPose::Close:
      painter.setBrush(QColor(0, 0xcd, 0));
      break;
    case HeadPose::Open:
      painter.setBrush(QColor(0xff, 0xff, 0));
      break;
    case HeadPose::OpenBig:
      painter.setBrush(QColor(0xff, 0, 0));
      break;
    }

    painter.drawEllipse(static_cast<int>(m_Status->pose.rotationZ * -10.0) + 242,
                        static_cast<int>(m_Status->pose.rotationX * -10.0) + 242,
                        16,
                        16);

    if (m_Status->pose.screenControlStatus == HeadPose::Soundwave) {
      painter.setBrush(QColor(0, 0xcd, 0));
    } else {
      painter.setBrush(QColor(0xcd, 0, 0));
    }
    painter.drawEllipse(static_cast<int>(m_Status->pose.rotationZ * -10.0) + 245,
                        static_cast<int>(m_Status->pose.rotationX * -10.0) + 245,
                        10,
                        10);
  }
}

void ManualOperateWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Q:
      m_Status->pose.mouthStatus =
        static_cast<HeadPose::MouthStatus>((m_Status->pose.mouthStatus + 1) % 3);
      update();
      return;
    case Qt::Key_W:
      m_Status->pose.screenControlStatus =
        static_cast<HeadPose::ScreenControlStatus>(-m_Status->pose.screenControlStatus);
      update();
      return;
    case Qt::Key_Escape:
      clearFocus();
      return;
  }

  QWidget::keyPressEvent(event);
}

void ManualOperateWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::RightButton) {
    clearFocus();
  } else {
    QWidget::mousePressEvent(event);
  }
}

bool ManualOperateWidget::event(QEvent *event) {
  switch (event->type()) {
    case QEvent::HoverMove:
      hoverMove(static_cast<QHoverEvent*>(event));
      return true;
    default:
      return QWidget::event(event);
  }
}

void ManualOperateWidget::hoverMove(QHoverEvent *event) {
  if (hasFocus()) {
    auto pos = event->oldPos();
    int dx = pos.x();
    int dy = pos.y();
    if (dx > 400) {
      dx = 400;
    } else if (dx < 100) {
      dx = 100;
    }
    if (dy > 400) {
      dy = 400;
    } else if (dy < 100) {
      dy = 100;
    }

    dx -= 250;
    dy -= 250;

    float yRotation = static_cast<float>(dx) / 10.0f;
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
