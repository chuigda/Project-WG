#include "include/wgc0310/head/Intake.h"

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "cwglx/drawable/TriangleGen.h"

namespace wgc0310 {

static std::unique_ptr<cw::TriangleGen> CreateThickCylinder();

static std::unique_ptr<cw::TriangleGen> CreateAirway();

std::unique_ptr<cw::TriangleGen> IntakeRight() {
  std::unique_ptr<cw::TriangleGen> thickCylinder = CreateThickCylinder();
  std::unique_ptr<cw::TriangleGen> thickCylinder1 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, 1.5));
  std::unique_ptr<cw::TriangleGen> thickCylinder2 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, -1.5));
  std::unique_ptr<cw::TriangleGen> thickCylinder3 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, 3.0));
  std::unique_ptr<cw::TriangleGen> thickCylinder4 =
      std::make_unique<cw::Positioner>(thickCylinder->Clone(),
                                       cw::Vector(0.0, 0.0, -3.0));
  std::unique_ptr<cw::TriangleGen> airway = CreateAirway();
  std::unique_ptr<cw::TriangleGen> airway1 =
      std::make_unique<cw::Positioner>(airway->Clone(),
                                       cw::Vector(0.0, 0.0, 0.75));
  std::unique_ptr<cw::TriangleGen> airway2 =
      std::make_unique<cw::Positioner>(airway->Clone(),
                                       cw::Vector(0.0, 0.0, -0.75));
  std::unique_ptr<cw::TriangleGen> airway3 =
      std::make_unique<cw::Positioner>(airway->Clone(),
                                       cw::Vector(0.0, 0.0, 2.25));
  std::unique_ptr<cw::TriangleGen> airway4 =
      std::make_unique<cw::Positioner>(airway->Clone(),
                                       cw::Vector(0.0, 0.0, -2.25));

  std::unique_ptr<cw::TriangleGen> frontBox =
      std::make_unique<cw::BoxGenerator>(cw::Vector(-0.125, 0.75, 0.0),
                                         0.25,
                                         1.5,
                                         6.75);
  std::unique_ptr<cw::TriangleGen> sideBox =
      std::make_unique<cw::BoxGenerator>(cw::Vector(1.375, -2.5, -0.0),
                                         0.25,
                                         5.0,
                                         6.75);

  std::vector<std::unique_ptr<cw::TriangleGen>> generators;
  generators.push_back(std::move(thickCylinder));
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
          cw::constants::g_ZeroVertex,
          cw::Axis::XAxis,
          90.0
      );

  return rotator;
}

static std::unique_ptr<cw::TriangleGen> CreateThickCylinder() {
  std::unique_ptr<cw::FanGenerator> fan1 =
      std::make_unique<cw::FanGenerator>(
          cw::Vector(0.0, 0.0, -0.375),
          1.5,
          90.0,
          0.0,
          32
      );

  std::unique_ptr<cw::FanGenerator> fan2 =
      std::make_unique<cw::FanGenerator>(
          cw::Vector(0.0, 0.0, 0.375),
          1.5,
          0.0,
          90.0,
          32
      );

  std::unique_ptr<cw::TriangleGen> thickCylinder =
      std::make_unique<cw::CylinderGenerator>(
          cw::constants::g_ZeroVector,
          1.5,
          0.75,
          0.0,
          90.0,
          32
      );

  std::vector<std::unique_ptr<cw::TriangleGen>> generators;
  generators.push_back(std::move(thickCylinder));
  generators.push_back(std::move(fan1));
  generators.push_back(std::move(fan2));

  return std::make_unique<cw::Composer>(std::move(generators));
}

static std::unique_ptr<cw::TriangleGen> CreateAirway() {
  std::unique_ptr<cw::ConvexPolyGenerator> topSurface =
      std::make_unique<cw::ConvexPolyGenerator>(
          std::vector<cw::Vertex> {
              cw::Vertex(1.25, 0.0, -0.375),
              cw::Vertex(0.0, 0.0, -0.375),
              cw::Vertex(-1.25, -2.5, -0.375),
              cw::Vertex(-1.25, -5.0, -0.375)
          }
      );

  std::unique_ptr<cw::ConvexPolyGenerator> bottomSurface =
      std::make_unique<cw::ConvexPolyGenerator>(
          std::vector<cw::Vertex> {
              cw::Vertex(1.25, 0.0, 0.375),
              cw::Vertex(-1.25, -5.0, 0.375),
              cw::Vertex(-1.25, -2.5, 0.375),
              cw::Vertex(0.0, 0.0, 0.375)
          }
      );

  std::unique_ptr<cw::ConvexPolyGenerator> leftSurface =
      std::make_unique<cw::ConvexPolyGenerator>(
          std::vector<cw::Vertex> {
              cw::Vertex(0.0, 0.0, -0.375),
              cw::Vertex(0.0, 0.0, 0.375),
              cw::Vertex(-1.25, -2.5, 0.375),
              cw::Vertex(-1.25, -2.5, -0.375)
          }
      );
  std::unique_ptr<cw::ConvexPolyGenerator> rightSurface =
      std::make_unique<cw::ConvexPolyGenerator>(
          std::vector<cw::Vertex> {
              cw::Vertex(1.25, 0.0, -0.375),
              cw::Vertex(-1.25, -5.0, -0.375),
              cw::Vertex(-1.25, -5.0, 0.375),
              cw::Vertex(1.25, 0.0, 0.375),
          }
      );

  std::unique_ptr<cw::StoredTriangles> blocker =
      std::make_unique<cw::StoredTriangles>(
          std::vector<std::array<cw::Vertex, 3>> {
              // first
              {
                  cw::Vertex { -1.25, -2.5, 0.375 },
                  cw::Vertex { -1.375, -3.0, 0.375 },
                  cw::Vertex { -1.375, -3.0, -0.375 },
              },
              {
                  cw::Vertex { -1.25, -2.5, 0.375 },
                  cw::Vertex { -1.375, -3.0, -0.375 },
                  cw::Vertex { -1.25, -2.5, -0.375 }
              },
              // second
              {
                  cw::Vertex { -1.25, -3.0, 0.375 },
                  cw::Vertex { -1.375, -3.5, 0.375 },
                  cw::Vertex { -1.375, -3.5, -0.375 },
              },
              {
                  cw::Vertex { -1.25, -3.0, 0.375 },
                  cw::Vertex { -1.375, -3.5, -0.375 },
                  cw::Vertex { -1.25, -3.0, -0.375 }
              },
              // third
              {
                  cw::Vertex { -1.25, -3.5, 0.375 },
                  cw::Vertex { -1.375, -4.0, 0.375 },
                  cw::Vertex { -1.375, -4.0, -0.375 },
              },
              {
                  cw::Vertex { -1.25, -3.5, 0.375 },
                  cw::Vertex { -1.375, -4.0, -0.375 },
                  cw::Vertex { -1.25, -3.5, -0.375 }
              },
              // fourth
              {
                  cw::Vertex { -1.25, -4.0, 0.375 },
                  cw::Vertex { -1.375, -4.5, 0.375 },
                  cw::Vertex { -1.375, -4.5, -0.375 },
              },
              {
                  cw::Vertex { -1.25, -4.0, 0.375 },
                  cw::Vertex { -1.375, -4.5, -0.375 },
                  cw::Vertex { -1.25, -4.0, -0.375 }
              },
              // fifth
              {
                  cw::Vertex { -1.25, -4.5, 0.375 },
                  cw::Vertex { -1.375, -5.0, 0.375 },
                  cw::Vertex { -1.375, -5.0, -0.375 },
              },
              {
                  cw::Vertex { -1.25, -4.5, 0.375 },
                  cw::Vertex { -1.375, -5.0, -0.375 },
                  cw::Vertex { -1.25, -4.5, -0.375 }
              }
          }
      );

  std::vector<std::unique_ptr<cw::TriangleGen>> generators;
  generators.push_back(std::move(topSurface));
  generators.push_back(std::move(bottomSurface));
  generators.push_back(std::move(leftSurface));
  generators.push_back(std::move(rightSurface));
  generators.push_back(std::move(blocker));

  return std::make_unique<cw::Composer>(std::move(generators));
}

} // namespace wgc0310
