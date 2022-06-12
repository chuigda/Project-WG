#include "cwglx/DrawableArena.h"

#include <QDebug>

namespace cw {

DrawableArena::DrawableArena() : m_CurrentId(0) {}

std::size_t DrawableArena::Put(std::unique_ptr<Drawable> &&drawable) {
  while (m_ReservedIds.find(m_CurrentId) != m_ReservedIds.end()) {
    ++m_CurrentId;
  }

  m_Drawables[m_CurrentId] = std::move(drawable);
  return m_CurrentId++;
}

std::size_t DrawableArena::Put(std::unique_ptr<Drawable> &&drawable,
                                std::size_t id) {
  Q_ASSERT(id < m_CurrentId && "id is already in use");
  Q_ASSERT(m_ReservedIds.find(id) != m_ReservedIds.end()
           && "id is already in use");

  m_Drawables[id] = std::move(drawable);
  m_ReservedIds.insert(id);
  return id;
}

const Drawable *DrawableArena::Get(std::size_t id) const {
  auto it = m_Drawables.find(id);
  if (it == m_Drawables.end()) {
    qCritical() << "Drawable with id" << id << "not found";
    return nullptr;
  }

  return it->second.get();
}

} // namespace cw
