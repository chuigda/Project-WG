#include <QApplication>
#include <QFile>
#include "ui_next/LicensePresenter.h"
#include "ui_next/ControlPanel.h"
#include "util/FileUtil.h"

std::pair<QDialog::DialogCode, LicensePresenter*>
PrecheckLicense() {
  QString mioText = cw::ReadToString(":/LICENSE-MIO-PHOENIX");

  LicensePresenter *presenter = new LicensePresenter();
  presenter->AddLicense(LicenseContent { "缪/菲尼克斯 特别许可证", mioText });

  if (QFile::exists("LICENSE.AGREED")) {
    return std::make_pair(QDialog::Accepted, presenter);
  }

  QDialog::DialogCode ret = static_cast<QDialog::DialogCode>(presenter->exec());
  if (ret == QDialog::Accepted) {
    QFile f("LICENSE.AGREED");
    f.open(QIODevice::WriteOnly);
    f.write("LICENSE AGREED");
  }
  return std::make_pair(ret, presenter);
}

int main(int argc, char *argv[]) {
  QApplication a { argc, argv };
  QApplication::setWindowIcon(QIcon(QPixmap(":/icon-v2.png")));

  QString startHideGLEnv = QString::fromLocal8Bit(qgetenv("START_HIDE_GL"));
  bool startHideGL = !startHideGLEnv.trimmed().isEmpty();

  auto [agreed, presenter] = PrecheckLicense();

  presenter->RequireAgreement(false);
  ControlPanel panel { presenter, startHideGL };
  panel.show();

  return QApplication::exec();
}
