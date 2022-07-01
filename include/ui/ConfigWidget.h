#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>
#include "cwglx/GLInfo.h"

namespace Ui {
class ConfigWidget;
} // namespace Ui

class ConfigWidget final : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigWidget(QWidget *parent = nullptr);
  ~ConfigWidget() final;

  void FillGLInfo(const cw::GLInfo &info);

private:
  Ui::ConfigWidget *ui;
};

#endif // CONFIGWIDGET_H
