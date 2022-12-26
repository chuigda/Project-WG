#ifndef PROJECT_WG_WGC0310_MESH_H
#define PROJECT_WG_WGC0310_MESH_H

#include <cstdint>
#include <memory>

#include "cwglx/drawable/Composition.h"
#include "cwglx/DrawableArena.h"

namespace wgc0310 {

class WGCMeshCollection {
public:
  WGCMeshCollection();
  WGCMeshCollection(GLFunctions *f, cw::DrawableArena &arena);

  void Load(GLFunctions *f, cw::DrawableArena &arena);

  // 头部模型
  cw::Drawable const* monitor;
  cw::Drawable const* monitorIntake;
  cw::Drawable const* monitorStroke;
  cw::Drawable const* wheel;

  // 胸部模型
  cw::Drawable const* chestBox;
  cw::Drawable const* chestBoxStroke;
  cw::Drawable const* chestPlate;
  cw::Drawable const* power;
  cw::Drawable const* powerPin;
  cw::Drawable const* colorTimer;
  cw::Drawable const* colorTimerShell;

  // 腹部模型
  cw::Drawable const* abdomen;
  cw::Drawable const* waist;

  // 肩部模型
  cw::Drawable const* shoulder;
  cw::Drawable const* shoulderPlate;

  // 胳膊模型
  cw::Drawable const* bigArm;
  cw::Drawable const* bigArmCover;
  cw::Drawable const* bigArmConnector;
  cw::Drawable const* smallArm;
  cw::Drawable const* smallArmCover;
  cw::Drawable const* wheelSmall;
  cw::Drawable const* claw;
  cw::Drawable const* clawCover;
};

} // namespace wgc0310

#endif /* PROJECT_WG_WGC0310_MESH_H */
