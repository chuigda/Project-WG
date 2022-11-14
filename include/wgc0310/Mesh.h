#ifndef PROJECT_WG_WGC0310_MESH_H
#define PROJECT_WG_WGC0310_MESH_H

#include <cstdint>
#include <memory>

#include "cwglx/drawable/Composition.h"
#include "cwglx/DrawableArena.h"

namespace wgc0310 {

class WGCMeshCollection {
public:
  WGCMeshCollection(GLFunctions *f, cw::DrawableArena &arena);

  // 头部模型
  cw::Drawable const* monitor;
  cw::Drawable const* monitorIntake;
  cw::Drawable const* radar;

  // 胸部模型
  cw::Drawable const* chestBox;
  cw::Drawable const* chestPlate;
  cw::Drawable const* power;
  cw::Drawable const* powerPin;
  cw::Drawable const* ber;
  cw::Drawable const* berShell;

  // 肩部模型
  cw::Drawable const* shoulder;
  cw::Drawable const* shoulderPlate;

  // 胳膊模型

};

} // namespace wgc0310

#endif /* PROJECT_WG_WGC0310_MESH_H */
