#ifndef PROJECT_WG_WGC0310_SCREEN_H
#define PROJECT_WG_WGC0310_SCREEH_H

#include "cwglx/Drawable.h"

namespace wgc0310 {

class ScreenImpl;

class Screen final : public cw::Drawable {
public:
  Screen(QOpenGLFunctions_2_0 *f);
  ~Screen() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

  void Delete(QOpenGLFunctions_2_0 *f) const noexcept final;

private:
  ScreenImpl *m_Impl;
};

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_SCREEN_H
