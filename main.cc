#include <QApplication>
#include "ui/GLWidget.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  GLWidget w;
  w.show();
  return a.exec();
}
