#include "util/FileUtil.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

namespace cw {

QString ReadToString(QString const& fileName) {
  QFile f(fileName);
  f.open(QIODevice::ReadOnly);
  if (!f.isOpen()) {
    qCritical() << "ReadToString(QString const&): cannot read from file"
                << fileName;
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
