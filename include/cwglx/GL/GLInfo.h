#ifndef PROJECT_GL2_INFO_H
#define PROJECT_GL2_INFO_H

#include <QString>

#include "cwglx/GL/GL.h"

namespace cw {

class GLInfo {
public:
  GLInfo(const char *vendor,
         const char *version,
         const char *renderer,
         QString extensions);

  const QString vendor;
  const QString version;
  const QString renderer;
  const QString extensions;

  static GLInfo AutoDetect(GLFunctions *f);
};

} // namespace cw

#endif // PROJECT_GL2_INFO_H
