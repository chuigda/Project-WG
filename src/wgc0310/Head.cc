#include "wgc0310/Head.h"

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/DrawableArena.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "cwglx/drawable/TriangleGenerator.h"

namespace wgc0310 {

static std::unique_ptr<cw::TriangleGenerator> CreateThickCylinder();
static std::unique_ptr<cw::TriangleGenerator> CreateThinCylinder();

cw::Drawable const* SideThermalController(cw::DrawableArena *arena) {
  std::unique_ptr<cw::TriangleGenerator> thickCylinder = CreateThickCylinder();
  std::unique_ptr<cw::TriangleGenerator> thinCylinder = CreateThinCylinder();

  std::unique_ptr<cw::TriangleGenerator> thickCylinder1 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, 2.0));
  std::unique_ptr<cw::TriangleGenerator> thickCylinder2 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, -2.0));
  std::unique_ptr<cw::TriangleGenerator> thinCylinder1 =
      std::make_unique<cw::Positioner>(thinCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, 3.75));
  std::unique_ptr<cw::TriangleGenerator> thinCylinder2 =
      std::make_unique<cw::Positioner>(std::move(thinCylinder),
                                       cw::Vector(0.0, 0.0, -3.75));


}

const cw::FanGenerator g_Fan = cw::FanGenerator(
    cw::Vector(0.0, 0.0, 0.0),
    3.0,
    0.0,
    90.0,
    24
);

static std::unique_ptr<cw::TriangleGenerator> CreateThickCylinder() {
  std::unique_ptr<cw::TriangleGenerator> thickCylinder =
      std::make_unique<cw::CylinderGenerator>(
          cw::constants::g_ZeroVector,
          3.0,
          1.0,
          0.0,
          90.0,
          24
      );

  std::unique_ptr<cw::TriangleGenerator> fan1 = g_Fan.Clone();
  std::unique_ptr<cw::TriangleGenerator> fan2 = g_Fan.Clone();

  std::unique_ptr<cw::TriangleGenerator> fan1Position =
      std::make_unique<cw::Positioner>(g_Fan.Clone(),
                                       cw::Vector(0.0, 0.0, -0.5));
  std::unique_ptr<cw::TriangleGenerator> fan2Position =
      std::make_unique<cw::Positioner>(g_Fan.Clone(),
                                       cw::Vector(0.0, 0.0, 0.5));

  std::vector<std::unique_ptr<cw::TriangleGenerator>> generators;
  generators.push_back(std::move(thickCylinder));
  generators.push_back(std::move(fan1));
  generators.push_back(std::move(fan2));

  return std::make_unique<cw::Composer>(std::move(generators));
}

static std::unique_ptr<cw::TriangleGenerator> CreateThinCylinder() {
  std::unique_ptr<cw::TriangleGenerator> thinCylinder =
      std::make_unique<cw::CylinderGenerator>(
          cw::constants::g_ZeroVector,
          3.0,
          0.5,
          0.0,
          90.0,
          24
      );

  std::unique_ptr<cw::TriangleGenerator> fan1 = g_Fan.Clone();
  std::unique_ptr<cw::TriangleGenerator> fan2 = g_Fan.Clone();

  std::unique_ptr<cw::TriangleGenerator> fan1Position =
      std::make_unique<cw::Positioner>(g_Fan.Clone(),
                                       cw::Vector(0.0, 0.0, -0.5));
  std::unique_ptr<cw::TriangleGenerator> fan2Position =
      std::make_unique<cw::Positioner>(g_Fan.Clone(),
                                       cw::Vector(0.0, 0.0, 0.5));

  std::vector<std::unique_ptr<cw::TriangleGenerator>> generators;
  generators.push_back(std::move(thinCylinder));
  generators.push_back(std::move(fan1));
  generators.push_back(std::move(fan2));

  return std::make_unique<cw::Composer>(std::move(generators));
}

} // namespace wgc0310
