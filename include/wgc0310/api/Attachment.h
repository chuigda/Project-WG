#ifndef PROJECT_WG_WGC0310_ATTACHMENT_H
#define PROJECT_WG_WGC0310_ATTACHMENT_H

#include "cwglx/GL.h"
#include "WGAPI.h"

class QWidget;

namespace wgc0310 {

class WGAPIAttachment {
public:
  virtual ~WGAPIAttachment() = default;

  virtual const char *GetName() noexcept = 0;

  virtual QWidget *GetControlWidget() noexcept = 0;

  virtual void NextTick() noexcept = 0;

  virtual bool Initialize(GLFunctions *f) noexcept = 0;

  virtual void Draw(GLFunctions *f) noexcept = 0;

  virtual void Delete(GLFunctions *f) noexcept = 0;
};

extern "C" {
using LoadAttachmentFn = WGAPIAttachment *WGAPI (*)();
} // extern "C"

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_ATTACHMENT_H
