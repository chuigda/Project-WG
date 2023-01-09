#ifndef PROJECT_WG_UI_NEXT_CLOSE_SIGNALLING_WIDGET_H
#define PROJECT_WG_UI_NEXT_CLOSE_SIGNALLING_WIDGET_H

#include <QWidget>

class CloseSignallingWidget : public QWidget {
  Q_OBJECT

public:
  explicit CloseSignallingWidget(QWidget *parent = nullptr,
                                 Qt::WindowType windowType = Qt::Window)
    : QWidget(parent, windowType)
  {}
  ~CloseSignallingWidget() override = default;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void AboutToHideOrClose();
  void AboutToClose();
#pragma clang diagnostic pop

protected:
  void closeEvent(QCloseEvent *event) override {
    QWidget::closeEvent(event);
    emit AboutToHideOrClose();
    emit AboutToClose();
  }

  void hideEvent(QHideEvent *event) override {
    QWidget::hideEvent(event);
    emit AboutToHideOrClose();
  }
};

#endif // PROJECT_WG_UI_NEXT_CLOSE_SIGNALLING_WIDGET_H
