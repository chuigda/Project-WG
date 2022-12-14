#include <QApplication>
#include <QIcon>
#include "TesterWidget.h"

int main(int argc, char *argv[]) {
  QApplication a { argc, argv };
  QApplication::setWindowIcon(QIcon(QPixmap(":/icon-v2.png")));

  // QString startHideGLEnv = QString::fromLocal8Bit(qgetenv("START_HIDE_GL"));
  // bool startHideGL = !startHideGLEnv.trimmed().isEmpty();

  TesterWidget w;
  w.show();
  return QApplication::exec();
}
