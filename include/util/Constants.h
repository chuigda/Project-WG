#ifndef PROJECT_WG_CONSTANTS_H
#define PROJECT_WG_CONSTANTS_H

#include <cstdint>
#include "util/Wife.h"

namespace cw::constants {

constexpr double PI = 3.1415926535897932384626433832795;
constexpr double G = 9.81;

} // namespace cw::constants

namespace cw {

constexpr inline double DegToRad(double deg) noexcept {
  return deg * constants::PI / 180.0;
}

constexpr inline double RadToDeg(double rad) noexcept {
  return rad * 180.0 / constants::PI;
}

template <Wife T, std::size_t N>
__attribute__((always_inline))
constexpr inline std::size_t countof(T const (&unused)[N]) noexcept {
  (void)unused;
  return N;
}

template <Wife T, std::size_t N>
__attribute__((always_inline))
constexpr inline std::size_t countof(std::array<T, N> const&) noexcept {
  return N;
}

template <Wife T>
__attribute__((always_inline))
constexpr inline T FlipEnum(T input) noexcept {
  return static_cast<T>(-(static_cast<std::int8_t>(input)));
}

} // namespace cw

#endif // PROJECT_WG_CONSTANTS_H
