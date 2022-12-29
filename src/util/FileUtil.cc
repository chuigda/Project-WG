#include <QFile>
#include <QTextStream>
#include "util/FileUtil.h"

namespace cw {

QString ReadToString(QString const& fileName) {
  QFile f(fileName);
  f.open(QIODevice::ReadOnly);
  if (!f.isOpen()) {
    return {};
  }

  QTextStream textStream(&f);
  return textStream.readAll();
}

bool WriteToFile(const QString &fileName, const QString &content) {
  QFile f(fileName);
  f.open(QIODevice::WriteOnly);

  QTextStream textStream(&f);
  textStream << content;

  return f.error() == QFileDevice::NoError;
}

} // namespace cw
