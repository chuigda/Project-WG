#include "ui/ConfigWidget.h"
#include "ui_ConfigWidget.h"

ConfigWidget::ConfigWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ConfigWidget)
{
  ui->setupUi(this);
}

ConfigWidget::~ConfigWidget() {
  delete ui;
}

void ConfigWidget::FillGLInfo(const cw::GLInfo &info) {
  ui->glVendorText->setText(info.vendor);
  ui->glVersionText->setText(info.version);
  ui->glRendererText->setText(info.renderer);
  ui->glExtensionsText->setPlainText(info.extensions);
}
