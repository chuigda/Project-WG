#include "cwglx/GLInfo.h"

#include "cwglx/GLImpl.h"
#include "ui_next/GLInfoDisplay.h"


namespace cw {

GLInfo::GLInfo(const char *vendor,
               const char *version,
               const char *renderer,
               const char *extensions)
  : vendor(vendor),
    version(version),
    renderer(renderer),
    extensions(extensions)
{}

GLInfo GLInfo::AutoDetect(GLFunctions *f) {
  const char *vendor =
      reinterpret_cast<const char *>(f->glGetString(GL_VENDOR));
  const char *version =
      reinterpret_cast<const char *>(f->glGetString(GL_VERSION));
  const char *renderer =
      reinterpret_cast<const char *>(f->glGetString(GL_RENDERER));
  const char *extensions =
      reinterpret_cast<const char *>(f->glGetString(GL_EXTENSIONS));
  return GLInfo { vendor, version, renderer, extensions };
}

} // namespace cw
