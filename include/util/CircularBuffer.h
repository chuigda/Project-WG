#ifndef PROJECT_WG_CIRCULAR_BUFFER_H
#define PROJECT_WG_CIRCULAR_BUFFER_H

#include <array>
#include <cstddef>
#include "util/Wife.h"

namespace cw {

template <Wife T, std::size_t N>
class CircularBuffer {
public:
  constexpr inline CircularBuffer() : m_First(0), m_Last(0) {}
  explicit constexpr inline
  CircularBuffer(T const& fillValue) : m_Buffer { fillValue, N }, m_First(0), m_Last(0) {}

  [[nodiscard]] constexpr inline std::size_t Size() const noexcept {
    if (m_Last >= m_First) {
      return m_Last - m_First;
    } else {
      return (N - m_First) + m_Last;
    }
  }

  constexpr inline void PushBack(T value) noexcept {
    m_Buffer[m_Last] = value;
    m_Last = (m_Last + 1) % N;
  }

  constexpr inline T PopFront() noexcept {
    T r = m_Buffer[m_First];
    m_First = (m_First + 1) % N;
  }

private:
  std::array<T, N> m_Buffer;
  std::size_t m_First;
  std::size_t m_Last;
};

} // namespace cw

#endif // PROJECT_WG_CIRCULAR_BUFFER_H
