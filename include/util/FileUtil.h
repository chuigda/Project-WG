#ifndef PROJECT_WG_FILEUTIL_H
#define PROJECT_WG_FILEUTIL_H

#include <QString>

namespace cw {

QString ReadToString(const char *fileName);

QString ReadToString(QString const &fileName);

} // namespace cw

#endif // PROJECT_WG_FILEUTIL_H
