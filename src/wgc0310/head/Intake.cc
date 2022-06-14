#include "include/wgc0310/head/Intake.h"

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/DrawableArena.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "cwglx/drawable/TriangleGenerator.h"
#include "wgc0310/Components.h"

namespace wgc0310 {

static std::unique_ptr<cw::TriangleGenerator> CreateThickCylinder();

static std::unique_ptr<cw::TriangleGenerator> CreateAirway();

std::unique_ptr<cw::TriangleGenerator> IntakeRight() {
  std::unique_ptr<cw::TriangleGenerator> thickCylinder1 =
      std::make_unique<cw::Positioner>(CreateThickCylinder(),
                                       cw::Vector(0.0, 0.0, 2.0));
  std::unique_ptr<cw::TriangleGenerator> thickCylinder2 =
      std::make_unique<cw::Positioner>(CreateThickCylinder(),
                                       cw::Vector(0.0, 0.0, -2.0));
  std::unique_ptr<cw::TriangleGenerator> thickCylinder3 =
      std::make_unique<cw::Positioner>(CreateThickCylinder(),
                                       cw::Vector(0.0, 0.0, 4.0));
  std::unique_ptr<cw::TriangleGenerator> thickCylinder4 =
      std::make_unique<cw::Positioner>(CreateThickCylinder(),
                                       cw::Vector(0.0, 0.0, -4.0));
  std::unique_ptr<cw::TriangleGenerator> airway = CreateAirway();
  std::unique_ptr<cw::TriangleGenerator> airway1 =
      std::make_unique<cw::Positioner>(airway->Clone(),
                                       cw::Vector(0.0, 0.0, 1.0));
  std::unique_ptr<cw::TriangleGenerator> airway2 =
      std::make_unique<cw::Positioner>(airway->Clone(),
                                       cw::Vector(0.0, 0.0, -1.0));
  std::unique_ptr<cw::TriangleGenerator> airway3 =
      std::make_unique<cw::Positioner>(airway->Clone(),
                                       cw::Vector(0.0, 0.0, 3.0));
  std::unique_ptr<cw::TriangleGenerator> airway4 =
      std::make_unique<cw::Positioner>(airway->Clone(),
                                       cw::Vector(0.0, 0.0, -3.0));

  std::unique_ptr<cw::TriangleGenerator> frontBox =
      std::make_unique<cw::BoxGenerator>(cw::Vector(-1.0, 1.5, 0.0),
                                         2.0,
                                         3.0,
                                         9.0);
  std::unique_ptr<cw::TriangleGenerator> sideBox =
      std::make_unique<cw::BoxGenerator>(cw::Vector(2.5, -3, -0.0),
                                         1.0,
                                         6.0,
                                         9.0);

  std::vector<std::unique_ptr<cw::TriangleGenerator>> generators;
  generators.push_back(std::move(CreateThickCylinder()));
  generators.push_back(std::move(thickCylinder1));
  generators.push_back(std::move(thickCylinder2));
  generators.push_back(std::move(thickCylinder3));
  generators.push_back(std::move(thickCylinder4));
  generators.push_back(std::move(airway1));
  generators.push_back(std::move(airway2));
  generators.push_back(std::move(airway3));
  generators.push_back(std::move(airway4));
  generators.push_back(std::move(frontBox));
  generators.push_back(std::move(sideBox));

  std::unique_ptr<cw::Composer> composer =
      std::make_unique<cw::Composer>(std::move(generators));

  std::unique_ptr<cw::Rotator> rotator =
      std::make_unique<cw::Rotator>(
          std::move(composer),
          cw::Vertex(0.0, 0.0, 0.0),
          cw::CircleAxis::XAxis,
          90.0
      );

  return rotator;
}

static std::unique_ptr<cw::TriangleGenerator> CreateThickCylinder() {
  std::unique_ptr<cw::FanGenerator> fan1 =
      std::make_unique<cw::FanGenerator>(
          cw::Vector(0.0, 0.0, -0.5),
          3.0,
          90.0,
          0.0,
          64
      );

  std::unique_ptr<cw::FanGenerator> fan2 =
      std::make_unique<cw::FanGenerator>(
          cw::Vector(0.0, 0.0, 0.5),
          3.0,
          0.0,
          90.0,
          64
      );

  std::unique_ptr<cw::TriangleGenerator> thickCylinder =
      std::make_unique<cw::CylinderGenerator>(
          cw::constants::g_ZeroVector,
          3.0,
          1.0,
          0.0,
          90.0,
          64
      );

  std::vector<std::unique_ptr<cw::TriangleGenerator>> generators;
  generators.push_back(std::move(thickCylinder));
  generators.push_back(std::move(fan1));
  generators.push_back(std::move(fan2));

  return std::make_unique<cw::Composer>(std::move(generators));
}

static std::unique_ptr<cw::TriangleGenerator> CreateAirway() {
  std::unique_ptr<cw::ConvexPolyGenerator> topSurface =
      std::make_unique<cw::ConvexPolyGenerator>(
          std::vector<cw::Vertex> {
              cw::Vertex(2.0, 0.0, -0.5),
              cw::Vertex(0.0, 0.0, -0.5),
              cw::Vertex(-2.0, -4.0, -0.5),
              cw::Vertex(-2.0, -8.0, -0.5)
          }
      );

  std::unique_ptr<cw::ConvexPolyGenerator> bottomSurface =
      std::make_unique<cw::ConvexPolyGenerator>(
          std::vector<cw::Vertex> {
              cw::Vertex(2.0, 0.0, 0.5),
              cw::Vertex(-2.0, -8.0, 0.5),
              cw::Vertex(-2.0, -4.0, 0.5),
              cw::Vertex(0.0, 0.0, 0.5)
          }
      );

  std::unique_ptr<cw::ConvexPolyGenerator> leftSurface =
      std::make_unique<cw::ConvexPolyGenerator>(
          std::vector<cw::Vertex> {
              cw::Vertex(0.0, 0.0, -0.5),
              cw::Vertex(0.0, 0.0, 0.5),
              cw::Vertex(-2.0, -4.0, 0.5),
              cw::Vertex(-2.0, -4.0, -0.5)
          }
      );
  std::unique_ptr<cw::ConvexPolyGenerator> rightSurface =
      std::make_unique<cw::ConvexPolyGenerator>(
          std::vector<cw::Vertex> {
              cw::Vertex(2.0, 0.0, -0.5),
              cw::Vertex(-2.0, -8.0, -0.5),
              cw::Vertex(-2.0, -8.0, 0.5),
              cw::Vertex(2.0, 0.0, 0.5),
          }
      );

  std::unique_ptr<cw::StoredTriangles> blocker =
      std::make_unique<cw::StoredTriangles>(
          std::vector<std::array<cw::Vertex, 3>> {
              {
                  cw::Vertex(-2.0, -4.0, -0.5),
                  cw::Vertex(-2.0, -4.0, 0.5),
                  cw::Vertex(-2.0, -5.0, 0.5),
              },
              {
                  cw::Vertex(-2.0, -5.0, -0.5),
                  cw::Vertex(-2.0, -6.0, 0.5),
                  cw::Vertex(-2.0, -6.0, -0.5),
              },
              {
                  cw::Vertex(-2.0, -6.0, -0.5),
                  cw::Vertex(-2.0, -6.0, 0.5),
                  cw::Vertex(-2.0, -7.0, 0.5),
              },
              {
                  cw::Vertex(-2.0, -7.0, -0.5),
                  cw::Vertex(-2.0, -8.0, 0.5),
                  cw::Vertex(-2.0, -8.0, -0.5),
              }
          }
      );

  std::vector<std::unique_ptr<cw::TriangleGenerator>> generators;
  generators.push_back(std::move(topSurface));
  generators.push_back(std::move(bottomSurface));
  generators.push_back(std::move(leftSurface));
  generators.push_back(std::move(rightSurface));
  generators.push_back(std::move(blocker));

  return std::make_unique<cw::Composer>(std::move(generators));
}

} // namespace wgc0310
