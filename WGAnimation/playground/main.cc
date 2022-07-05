#include <QApplication>
#include "PlaygroundWidget.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  PlaygroundWidget widget;
  widget.show();
  return a.exec();
}
