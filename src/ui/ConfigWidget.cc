#include "ui/ConfigWidget.h"
#include "ui_ConfigWidget.h"

ConfigWidget::ConfigWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ConfigWidget)
{
  ui->setupUi(this);
}

ConfigWidget::~ConfigWidget()
{
  delete ui;
}
