#ifndef PROJECT_WG_WGC0310_SCREEN_ANIMATION_H
#define PROJECT_WG_WGC0310_SCREEN_ANIMATION_H

#include "cwglx/GL.h"
#include "wgc0310/api/WGAPI.h"
#include "util/Derive.h"

class QWidget;

namespace wgc0310 {

class WGAPIAnimation {
public:
  WGAPIAnimation() = default;

  virtual ~WGAPIAnimation() = default;

  virtual const char *GetName() noexcept = 0;

  virtual QWidget *GetControlWidget() noexcept = 0;

  virtual void NextTick() noexcept = 0;

  virtual void Rewind() noexcept = 0;

  virtual bool Initialize(GLFunctions *f) noexcept = 0;

  virtual void Draw(GLFunctions *f) noexcept = 0;

  virtual void Delete(GLFunctions *f) noexcept = 0;

  CW_DERIVE_UNCOPYABLE(WGAPIAnimation)
  CW_DERIVE_UNMOVABLE(WGAPIAnimation)
};

extern "C" {
using LoadAnimationFn = WGAPIAnimation *WGAPI (*)();
} // extern "C"

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SCREEN_ANIMATION_H
