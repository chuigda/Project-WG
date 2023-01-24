#ifndef PROJECT_WG_WGC0310_SCREEN_H
#define PROJECT_WG_WGC0310_SCREEN_H

#include <functional>
#include <glm/fwd.hpp>
#include "cwglx/GL/GL.h"
#include "cwglx/Base/ShaderProgram.h"

namespace wgc0310 {

class ScreenImpl;

class Screen final {
public:
  explicit Screen(GLFunctions *f);
  ~Screen();

  void BeginScreenContext(GLFunctions *f) const noexcept;

  void DoneScreenContext(GLFunctions *f) const noexcept;

  void Draw(GLFunctions *f, cw::ShaderProgram *shaderProgram) const noexcept;

  void Delete(GLFunctions *f) const noexcept;

private:
  ScreenImpl *m_Impl;
};

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SCREEN_H
