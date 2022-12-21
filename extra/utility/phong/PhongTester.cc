#include <QApplication>
#include <QIcon>
#include "TesterWidget.h"

int main(int argc, char *argv[]) {
  QApplication a { argc, argv };
  QApplication::setWindowIcon(QIcon(QPixmap(":/phong-tester-icon.png")));

  TesterWidget w;
  w.show();
  return QApplication::exec();
}