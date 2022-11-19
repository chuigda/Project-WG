#include "ui/CaptureWidget.h"

#include <QPainter>

CaptureWidget::CaptureWidget(FaceTrackStatus *status, QWidget *parent)
  : QWidget(parent),
    m_Status(status)
{
  this->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  this->setMinimumSize(300, 300);
  this->setMaximumSize(300, 300);
  this->setFixedWidth(300);
  this->setFixedHeight(300);

  this->setStyleSheet("background-color: black;");
  this->setFocusPolicy(Qt::StrongFocus);
}

void CaptureWidget::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event)

  QPainter painter(this);
  painter.setBrush(QColor(255, 255, 255));
  painter.drawEllipse(0, 0, 300, 300);

  if (this->hasFocus()) {
    painter.setBrush(QColor(0xcd, 0, 0));
    painter.drawEllipse(145, 145, 10, 10);
  }
}
