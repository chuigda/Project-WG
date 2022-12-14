#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[]) {
  QApplication a { argc, argv };
  QApplication::setWindowIcon(QIcon(QPixmap(":/icon-v2.png")));

  // QString startHideGLEnv = QString::fromLocal8Bit(qgetenv("START_HIDE_GL"));
  // bool startHideGL = !startHideGLEnv.trimmed().isEmpty();

  return QApplication::exec();
}
