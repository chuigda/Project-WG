#include "cwglx/GL/GLInfo.h"
#include "cwglx/GL/GLImpl.h"

namespace cw {

GLInfo::GLInfo(const char *vendor,
               const char *version,
               const char *renderer,
               QString extensions)
  : vendor(vendor),
    version(version),
    renderer(renderer),
    extensions(std::move(extensions))
{}

GLInfo GLInfo::AutoDetect(GLFunctions *f) {
  const char *vendor = reinterpret_cast<const char *>(f->glGetString(GL_VENDOR));
  const char *version = reinterpret_cast<const char *>(f->glGetString(GL_VERSION));
  const char *renderer = reinterpret_cast<const char *>(f->glGetString(GL_RENDERER));

  GLint numExtensions = 0;
  f->glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

  QStringList extensions;
  for (GLint i = 0; i < numExtensions; i++) {
    const char *extension =
      reinterpret_cast<const char *>(f->glGetStringi(GL_EXTENSIONS, i));
    extensions.push_back(extension);
  }

  return GLInfo { vendor, version, renderer, extensions.join(' ') };
}

} // namespace cw
