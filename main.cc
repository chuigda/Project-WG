#include "ui/GLWidget.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  GLWidget w;
  w.show();
  return a.exec();
}
