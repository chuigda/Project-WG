#include <QApplication>
#include "TypeWriterControl.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  TypeWriterControl c;
  c.show();

  return QApplication::exec();
}
