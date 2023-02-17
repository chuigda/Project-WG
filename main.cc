#include <QApplication>
#include <QFile>
#include <QSplashScreen>
#include <QTimer>
#include <QFontDatabase>
#include <QIcon>

#include "GlobalConfig.h"
#include "ui_next/LicensePresenter.h"
#include "ui_next/ControlPanel.h"
#include "util/FileUtil.h"

std::pair<QDialog::DialogCode, LicensePresenter*>
PrecheckLicense() {
  QString preamble = cw::ReadToString(":/PREAMBLE");
  QString agplText = cw::ReadToString(":/LICENSE");
  QString ccText = cw::ReadToString(":/LICENSE-CC-BY-SA");
  QString agplBrief = cw::ReadToString(":/AGPL-BRIEF");
  QString ccBrief = cw::ReadToString(":/CC-BY-SA-BRIEF");

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
    "署名-相同方式共享 4.0 国际",
    ccText,
    ccBrief,
    "https://creativecommons.org/licenses/by-sa/4.0/deed.zh"
  });

  QString forceReadLicenseEnv = QString::fromLocal8Bit(qgetenv("FORCE_READ_LICENSE"));
  if (QFile::exists("LICENSE.AGREED") &&
      forceReadLicenseEnv.trimmed().isEmpty()) {
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
  cw::InitGlobalConfig();

  QApplication a { argc, argv };
  QApplication::setWindowIcon(QIcon(QPixmap(":/icon-v2.png")));

  QFontDatabase::addApplicationFont(":/material-icons.ttf");

  QString startHideGLEnv = QString::fromLocal8Bit(qgetenv("START_HIDE_GL"));
  bool startHideGL = !startHideGLEnv.trimmed().isEmpty();

  auto [agreed, presenter] = PrecheckLicense();
  if (agreed != QDialog::Accepted) {
    delete presenter;
    return 0;
  }

  QPixmap splashPixmap = QPixmap(":/splash.png");
  qreal dpi = a.devicePixelRatio();
  QSplashScreen splash(splashPixmap.scaled(640.0 / dpi, 480.0 / dpi));
  splash.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  splash.setWindowFlag(Qt::SplashScreen, false);
  splash.setEnabled(false);
  splash.show();

  ControlPanel panel { startHideGL };

  QTimer::singleShot(500, [&] {
    splash.close();

    panel.show();
    panel.DoneSplash();
  });

  return QApplication::exec();
}
