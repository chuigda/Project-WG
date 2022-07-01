#ifndef PROJECT_WG_GL_INFO_H
#define PROJECT_WG_GL_INFO_H

#include <QString>

#include "cwglx/GL.h"

namespace cw {

class GLInfo {
public:
  GLInfo(const char *vendor,
         const char *version,
         const char *renderer,
         const char *extensions);

  const QString vendor;
  const QString version;
  const QString renderer;
  const QString extensions;

  static GLInfo AutoDetect(GLFunctions *f);
};

} // namespace cw

#endif // PROJECT_WG_GL_INFO_H
