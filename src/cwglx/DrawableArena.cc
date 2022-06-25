#include "cwglx/DrawableArena.h"

#include <QDebug>

namespace cw {

DrawableArena::DrawableArena() : m_CurrentId(0) {}

std::pair<std::size_t, Drawable const*>
DrawableArena::Put(std::unique_ptr<Drawable> &&drawable) {
  while (m_ReservedIds.find(m_CurrentId) != m_ReservedIds.end()) {
    ++m_CurrentId;
  }

  const auto [it, _] =
      m_Drawables.insert(std::make_pair<>(m_CurrentId, std::move(drawable)));
  return std::make_pair(m_CurrentId++, it->second.get());
}

std::pair<std::size_t, Drawable const*>
DrawableArena::Put(std::unique_ptr<Drawable> &&drawable,
                                std::size_t id) {
  Q_ASSERT(id >= m_CurrentId && "id is already in use");
  Q_ASSERT(m_ReservedIds.find(id) == m_ReservedIds.end()
           && "id is already in use");

  const auto [it, _] =
      m_Drawables.insert(std::make_pair<>(id, std::move(drawable)));
  m_ReservedIds.insert(id);
  return std::make_pair(id, it->second.get());
}

const Drawable *DrawableArena::Get(std::size_t id) const {
  auto it = m_Drawables.find(id);
  if (it == m_Drawables.end()) {
    qCritical() << "Drawable with id" << id << "not found";
    return nullptr;
  }

  return it->second.get();
}

void DrawableArena::Delete(GLFunctions *f) const noexcept {
  for (const auto &[_, drawable] : m_Drawables) {
    drawable->Delete(f);
  }
}

} // namespace cw
