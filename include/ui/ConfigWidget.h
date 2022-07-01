#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>

namespace Ui {
class ConfigWidget;
}

class ConfigWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigWidget(QWidget *parent = nullptr);
  ~ConfigWidget();

private:
  Ui::ConfigWidget *ui;
};

#endif // CONFIGWIDGET_H
