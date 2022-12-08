#ifndef PROJECT_WG_WGC0310_SCREEN_ANIMATION_H
#define PROJECT_WG_WGC0310_SCREEN_ANIMATION_H

#include "cwglx/GL.h"
#include "WGAPI.h"

class QWidget;

namespace wgc0310 {

class WGAPIAnimation {
public:
  virtual ~WGAPIAnimation() = default;

  virtual const char *GetName() noexcept = 0;

  virtual QWidget *GetControlWidget() noexcept = 0;

  virtual void NextTick() noexcept = 0;

  virtual void Rewind() noexcept = 0;

  virtual bool Initialize(GLFunctions *f) noexcept = 0;

  virtual void Draw(GLFunctions *f) noexcept = 0;

  virtual void Delete(GLFunctions *f) noexcept = 0;
};

extern "C" {
using LoadAnimationFn = WGAPIAnimation *WGAPI (*)();
} // extern "C"

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SCREEN_ANIMATION_H
