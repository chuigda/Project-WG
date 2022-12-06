#include <QFile>
#include <QTextStream>
#include "util/FileUtil.h"

namespace cw {

QString ReadToString(const char *fileName) {
  return ReadToString(QString(fileName));
}

QString ReadToString(QString const& fileName) {
  QFile f(fileName);
  f.open(QIODevice::ReadOnly);

  QTextStream textStream(&f);
  return textStream.readAll();
}

} // namespace cw
