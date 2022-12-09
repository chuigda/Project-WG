#include <QApplication>
#include <QFile>
#include "ui_next/LicensePresenter.h"
#include "ui_next/ControlPanel.h"
#include "util/FileUtil.h"

std::pair<QDialog::DialogCode, LicensePresenter*>
PrecheckLicense() {
  QString preamble = cw::ReadToString(":/PREAMBLE");
  QString agplText = cw::ReadToString(":/LICENSE");
  QString ccText = cw::ReadToString(":/LICENSE-CC-BY-NC-SA");
  QString agplBrief = cw::ReadToString(":/AGPL-BRIEF");
  QString ccBrief = cw::ReadToString(":/CC-BY-NC-SA-BRIEF");

  LicensePresenter *presenter = new LicensePresenter();
  presenter->AddLicense(LicenseContent {
    "前言",
    preamble
  });

  presenter->AddLicense(LicenseContent {
    "GNU Affero 通用公共许可证 第三版",
    agplText,
    agplBrief,
    "https://www.gnu.org/licenses/agpl-3.0.zh-cn.html"
  });

  presenter->AddLicense(LicenseContent {
    "署名-非商业性使用-相同方式共享 4.0 国际",
    ccText,
    ccBrief,
    "https://creativecommons.org/licenses/by-nc-sa/4.0/deed.zh"
  });

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

  auto [agreed, presenter] = PrecheckLicense();
  if (agreed != QDialog::Accepted) {
    delete presenter;
    return 0;
  }

  presenter->RequireAgreement(false);
  ControlPanel panel { presenter };
  panel.show();

  return QApplication::exec();
}
