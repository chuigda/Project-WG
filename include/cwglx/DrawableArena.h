#ifndef PROJECT_WG_DRAWABLE_ARENA_H
#define PROJECT_WG_DRAWABLE_ARENA_H

#include <memory>
#include <vector>

#include "cwglx/GL.h"
#include "cwglx/Drawable.h"
#include "util/Derive.h"

namespace cw {

class DrawableArena {
public:
  DrawableArena();

  CW_DERIVE_UNCOPYABLE(DrawableArena)
  CW_DERIVE_UNMOVABLE(DrawableArena)

  [[nodiscard]] Drawable const* Put(std::unique_ptr<Drawable> &&drawable);

  void Delete(GLFunctions *f) const noexcept;

private:
  std::vector<std::unique_ptr<Drawable>> m_Drawables;
};

} // namespace cw

#endif // PROJECT_WG_DRAWABLE_ARENA_H
