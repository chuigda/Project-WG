#ifndef PROJECT_WG_CAPTURE_WIDGET
#define PROJECT_WG_CAPTURE_WIDGET

#include <QWidget>
#include "ui/FaceTrackStatus.h"

class CaptureWidget : public QWidget {
public:
  explicit CaptureWidget(FaceTrackStatus *status, QWidget *parent = nullptr);

  void paintEvent(QPaintEvent *event) override;

  void keyPressEvent(QKeyEvent *event) override;

  void keyReleaseEvent(QKeyEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

protected:
  bool event(QEvent *event) override;
  void hoverMove(QHoverEvent *event);

private:
  FaceTrackStatus *m_Status;
};

#endif // PROJECT_WG_CAPTURE_WIDGET
