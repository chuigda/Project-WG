#ifndef PROJECT_WG_WGC0310_SCREEN_H
#define PROJECT_WG_WGC0310_SCREEN_H

#include <functional>

#include "cwglx/GL.h"
#include "cwglx/Drawable.h"

namespace wgc0310 {

class ScreenImpl;

class Screen final : public cw::Drawable {
public:
  explicit Screen(GLFunctions *f);
  ~Screen() final;

  void BeginScreenContext(GLFunctions *f) const noexcept;

  void DoneScreenContext(GLFunctions *f) const noexcept;

  void Draw(GLFunctions *f) const noexcept final;

  void Delete(GLFunctions *f) const noexcept final;

private:
  ScreenImpl *m_Impl;
};

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SCREEN_H
