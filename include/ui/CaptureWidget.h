#ifndef PROJECT_WG_CAPTURE_WIDGET
#define PROJECT_WG_CAPTURE_WIDGET

#include <QWidget>
#include "ui/FaceTrackStatus.h"

class CaptureWidget : public QWidget {
public:
  explicit CaptureWidget(FaceTrackStatus *status, QWidget *parent = nullptr);

  void paintEvent(QPaintEvent *event) override;

private:
  FaceTrackStatus *m_Status;
};

#endif // PROJECT_WG_CAPTURE_WIDGET
