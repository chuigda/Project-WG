#ifndef PROJECT_WG_BIS_ANIM_H
#define PROJECT_WG_BIS_ANIM_H

#include <cstddef>
#include <cstdint>

namespace bis {

extern "C" struct Context {
  float in_x;
  float in_y;
  float in_z;

  std::int32_t frame;
};

} // namespace bis

#endif // PROJECT_WG_BIS_ANIM_H
