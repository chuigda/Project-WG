#ifndef PROJECT_WG_CONSTANTS_H
#define PROJECT_WG_CONSTANTS_H

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

} // namespace cw

#endif // PROJECT_WG_CONSTANTS_H
