#ifndef PROJECT_WG_FILEUTIL_H
#define PROJECT_WG_FILEUTIL_H

class QString;

namespace cw {

bool IsFileExists(QString const& fileName);

QString ReadToString(QString const &fileName);

bool WriteToFile(QString const& fileName, QString const& content);

} // namespace cw

#endif // PROJECT_WG_FILEUTIL_H
