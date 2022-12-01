#include "cwglx/DrawableArena.h"

#include <QDebug>

namespace cw {

DrawableArena::DrawableArena() = default;

Drawable const* DrawableArena::Put(std::unique_ptr<Drawable> &&drawable) {
  Drawable const* r = drawable.get();
  m_Drawables.push_back(std::move(drawable));
  return r;
}

void DrawableArena::Delete(GLFunctions *f) const noexcept {
  for (const auto &drawable : m_Drawables) {
    drawable->Delete(f);
  }
}

} // namespace cw
