#ifndef PROJECT_WG_DRAWABLE_ARENA_H
#define PROJECT_WG_DRAWABLE_ARENA_H

#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "cwglx/Drawable.h"
#include "util/Derive.h"

namespace cw {

class DrawableArena {
public:
  DrawableArena();

  CW_DERIVE_UNCOPYABLE(DrawableArena);
  CW_DERIVE_UNMOVABLE(DrawableArena);

  [[nodiscard]] std::size_t Put(std::unique_ptr<Drawable> &&drawable);

  std::size_t Put(std::unique_ptr<Drawable> &&drawable, std::size_t id);

  [[nodiscard]] Drawable const* Get(std::size_t id) const;

private:
  std::size_t m_CurrentId;
  std::unordered_set<std::size_t> m_ReservedIds;
  std::unordered_map<std::size_t, std::unique_ptr<Drawable>> m_Drawables;
};

} // namespace cw

#endif // PROJECT_WG_DRAWABLE_ARENA_H
