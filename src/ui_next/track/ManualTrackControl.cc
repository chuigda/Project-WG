#include "TrackControlImpl.h"

#include <QPainter>
#include <QKeyEvent>
#include <QBoxLayout>
#include <QLabel>
#include "wgc0310/HeadStatus.h"
#include "util/Constants.h"

using wgc0310::HeadStatus;
using wgc0310::ScreenDisplayMode;

class ManualTrackWidget : public QWidget {
public:
  explicit ManualTrackWidget(wgc0310::HeadStatus *headStatus,
                             wgc0310::ScreenDisplayMode *screenDisplayMode)
    : m_HeadStatus(headStatus),
      m_ScreenDisplayMode(screenDisplayMode)
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

  void paintEvent(QPaintEvent*) override {
    if (this->hasFocus()) {
      QPainter painter(this);

      painter.setBrush(QColor(0xff, 0xff, 0xff));
      painter.drawRect(100, 100, 300, 300);
      painter.drawRect(245, 245, 10, 10);

      if (m_HeadStatus->mouthStatus == HeadStatus::MouthStatus::Close) {
        painter.setBrush(QColor(0, 0xcd, 0));
      } else {
        painter.setBrush(QColor(0xcd, 0, 0));
      }

      painter.drawEllipse(static_cast<int>(m_HeadStatus->rotationZ * -10.0) + 242,
                          static_cast<int>(m_HeadStatus->rotationX * -10.0) + 242,
                          16,
                          16);

      if (*m_ScreenDisplayMode == ScreenDisplayMode::CapturedExpression) {
        painter.setBrush(QColor(0, 0xcd, 0));
      } else {
        painter.setBrush(QColor(0xcd, 0, 0));
      }
      painter.drawEllipse(static_cast<int>(m_HeadStatus->rotationZ * -10.0) + 245,
                          static_cast<int>(m_HeadStatus->rotationX * -10.0) + 245,
                          10,
                          10);
    }
  }

  void keyPressEvent(QKeyEvent *event) override {
    switch (event->key()) {
      case Qt::Key_Q:
        *m_ScreenDisplayMode = cw::FlipEnum(*m_ScreenDisplayMode);
        update();
        return;
      case Qt::Key_W:
        m_HeadStatus->mouthStatus = cw::FlipEnum(m_HeadStatus->mouthStatus);
        update();
        return;
      case Qt::Key_Escape:
        clearFocus();
        return;
    }

    QWidget::keyPressEvent(event);
  }

  void mousePressEvent(QMouseEvent *event) override {
    if (event->button() == Qt::RightButton) {
      clearFocus();
    } else {
      QWidget::mousePressEvent(event);
    }
  }

protected:
  bool event(QEvent *event) override {
    switch (event->type()) {
      case QEvent::HoverMove:
        hoverMove(static_cast<QHoverEvent*>(event));
        return true;
      default:
        return QWidget::event(event);
    }
  }

  void hoverMove(QHoverEvent *event) {
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

      m_HeadStatus->rotationY = m_HeadStatus->rotationY * 0.5f + yRotation * 0.5f;
      m_HeadStatus->rotationX = m_HeadStatus->rotationX * 0.5f + xRotation * 0.5f;
      m_HeadStatus->rotationZ = m_HeadStatus->rotationZ * 0.5f + zRotation * 0.5f;

      update();
    }
  }

private:
  wgc0310::HeadStatus *m_HeadStatus;
  wgc0310::ScreenDisplayMode *m_ScreenDisplayMode;
};


ManualTrackControl::ManualTrackControl(wgc0310::HeadStatus *headStatus,
                                       wgc0310::ScreenDisplayMode *screenDisplayMode,
                                       QWidget *parent)
  : QWidget(parent),
    m_HeadStatus(headStatus),
    m_ScreenDisplayMode(screenDisplayMode)
{
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addStretch();
  hbox->addWidget(new ManualTrackWidget(headStatus, screenDisplayMode));
  hbox->addStretch();

  QVBoxLayout *vbox = new QVBoxLayout();
  QLabel *introduction = new QLabel(
    "左键单击白色区域并使用鼠标操控，右键单击或者按 <code>ESC</code> 来停止操控。<br/>"
    "按 <code>Q</code> 来切换面部表情/音频分析，按 <code>W</code> 来切换口部状态"
  );
  vbox->addWidget(introduction);

  vbox->addStretch();
  vbox->addLayout(hbox);
  vbox->addStretch();

  setMinimumSize(600, 600);
  setLayout(vbox);
}
