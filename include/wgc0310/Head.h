#ifndef PROJECT_WG_WGC0310_HEAD_H
#define PROJECT_WG_WGC0310_HEAD_H

#include <utility>

namespace cw {

class Drawable;
class DrawableArena;

} // namespace cw

namespace wgc0310 {

cw::Drawable const* SideThermalController(cw::DrawableArena *arena);

} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_HEAD_H
