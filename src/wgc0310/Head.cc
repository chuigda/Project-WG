#include "wgc0310/Head.h"

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/DrawableArena.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "cwglx/drawable/TriangleGenerator.h"
#include "wgc0310/Components.h"

namespace wgc0310 {

static std::unique_ptr<cw::TriangleGenerator> CreateThickCylinder();

cw::Drawable const* SideThermalController(cw::DrawableArena *arena) {
  std::unique_ptr<cw::TriangleGenerator> thickCylinder = CreateThickCylinder();

  std::unique_ptr<cw::TriangleGenerator> thickCylinder1 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, 2.0));
  std::unique_ptr<cw::TriangleGenerator> thickCylinder2 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, -2.0));
  std::unique_ptr<cw::TriangleGenerator> thickCylinder3 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, 4.0));
  std::unique_ptr<cw::TriangleGenerator> thickCylinder4 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, -4.0));

  std::unique_ptr<cw::TriangleGenerator> frontBox =
      std::make_unique<cw::BoxGenerator>(cw::Vector(-1.0, 2.5, 0.0),
                                         2.0,
                                         1.0,
                                         9.0);
  std::unique_ptr<cw::TriangleGenerator> sideBox =
      std::make_unique<cw::BoxGenerator>(cw::Vector(2.5, -3, -0.0),
                                         1.0,
                                         6.0,
                                         9.0);

  std::vector<std::unique_ptr<cw::TriangleGenerator>> generators;
  generators.push_back(std::move(thickCylinder));
  generators.push_back(std::move(thickCylinder1));
  generators.push_back(std::move(thickCylinder2));
  generators.push_back(std::move(thickCylinder3));
  generators.push_back(std::move(thickCylinder4));
  generators.push_back(std::move(frontBox));
  generators.push_back(std::move(sideBox));

  std::unique_ptr<cw::Composer> composer =
      std::make_unique<cw::Composer>(std::move(generators));

  cw::Rotator rotator {
      std::move(composer),
      cw::Vertex(0.0, 0.0, 0.0),
      cw::CircleAxis::XAxis,
      90.0
  };

  std::unique_ptr<cw::PlainTriangles> triangles =
      std::make_unique<cw::PlainTriangles>();
  triangles->AddTriangles(&rotator);

  const auto [_, ptr] = arena->Put(std::move(triangles),
                                   wgc0310::Components::SideThermalController);
  return ptr;
}

const cw::FanGenerator g_Fan1 = cw::FanGenerator(
    cw::Vector(0.0, 0.0, 0.0),
    3.0,
    90.0,
    0.0,
    64
);

const cw::FanGenerator g_Fan2 = cw::FanGenerator(
    cw::Vector(0.0, 0.0, 0.0),
    3.0,
    0.0,
    90.0,
    64
);

static std::unique_ptr<cw::TriangleGenerator> CreateThickCylinder() {
  std::unique_ptr<cw::TriangleGenerator> thickCylinder =
      std::make_unique<cw::CylinderGenerator>(
          cw::constants::g_ZeroVector,
          3.0,
          1.0,
          0.0,
          90.0,
          64
      );

  std::unique_ptr<cw::TriangleGenerator> fan1Position =
      std::make_unique<cw::Positioner>(g_Fan1.Clone(),
                                       cw::Vector(0.0, 0.0, -0.5));
  std::unique_ptr<cw::TriangleGenerator> fan2Position =
      std::make_unique<cw::Positioner>(g_Fan2.Clone(),
                                       cw::Vector(0.0, 0.0, 0.5));

  std::vector<std::unique_ptr<cw::TriangleGenerator>> generators;
  generators.push_back(std::move(thickCylinder));
  generators.push_back(std::move(fan1Position));
  generators.push_back(std::move(fan2Position));

  return std::make_unique<cw::Composer>(std::move(generators));
}

} // namespace wgc0310
