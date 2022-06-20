#include "wgc0310/head/HeadFrame.h"

namespace wgc0310 {

std::unique_ptr<cw::TriangleGen> HeadFrame() {
  std::unique_ptr<cw::TriangleGen> verticalCylinder =
      std::make_unique<cw::CylinderGenerator>(
          cw::Vector(11.0, 0.0, 5.0),
          1.25,
          4.0,
          0.0,
          90.0,
          32
      );
  std::unique_ptr<cw::TriangleGen> horizontalCylinder =
      std::make_unique<cw::CylinderGenerator>(
          cw::Vector(7, 0.0, -5.5),
          1.25,
          11.0,
          0.0,
          90.0,
          32
      );
  std::unique_ptr<cw::TriangleGen> sphere =
      std::make_unique<cw::SphereGenerator>(
          cw::Vector(11.0, 0.0, 7.0),
          1.25,
          0.0,
          90.0,
          0.0,
          90.0,
          32,
          32
      );
  std::unique_ptr<cw::TriangleGen> rotatedHorizontalCylinder =
      std::make_unique<cw::Rotator>(
          std::move(horizontalCylinder),
          cw::constants::g_ZeroVertex,
          cw::Axis::YAxis,
          -90.0
      );

  std::vector<std::unique_ptr<cw::TriangleGen>> generators;
  generators.push_back(std::move(verticalCylinder));
  generators.push_back(std::move(rotatedHorizontalCylinder));
  generators.push_back(std::move(sphere));

  std::unique_ptr<cw::TriangleGen> rightBottomCorner =
      std::make_unique<cw::Composer>(std::move(generators));
  std::unique_ptr<cw::TriangleGen> leftBottomCorner =
      std::make_unique<cw::Flipper>(
          rightBottomCorner->Clone(),
          cw::Plane::YZPlane
      );
  std::unique_ptr<cw::TriangleGen> rightTopCorner =
      std::make_unique<cw::Flipper>(
          rightBottomCorner->Clone(),
          cw::Plane::XYPlane
      );
  std::unique_ptr<cw::TriangleGen> leftTopCorner =
      std::make_unique<cw::Flipper>(
          leftBottomCorner->Clone(),
          cw::Plane::XYPlane
      );
  std::unique_ptr<cw::TriangleGen> topBlock =
      std::make_unique<cw::BoxGenerator>(
          cw::Vector(0.0, 0.625, -6.75),
          22.0, 1.25, 0.5
      );
  std::unique_ptr<cw::TriangleGen> bottomBlock =
      std::make_unique<cw::BoxGenerator>(
          cw::Vector(0.0, 0.625, 6.75),
          22.0, 1.25, 0.5
      );
  std::unique_ptr<cw::TriangleGen> leftBlock =
      std::make_unique<cw::BoxGenerator>(
          cw::Vector(-10.75, 0.625, 0.0),
          0.5, 1.25, 13.0
      );
  std::unique_ptr<cw::TriangleGen> rightBlock =
      std::make_unique<cw::BoxGenerator>(
          cw::Vector(10.75, 0.625, 0.0),
          0.5, 1.25, 13.0
      );

  std::vector<std::unique_ptr<cw::TriangleGen>> generators2;
  generators2.push_back(std::move(rightBottomCorner));
  generators2.push_back(std::move(leftBottomCorner));
  generators2.push_back(std::move(rightTopCorner));
  generators2.push_back(std::move(leftTopCorner));
  generators2.push_back(std::move(topBlock));
  generators2.push_back(std::move(bottomBlock));
  generators2.push_back(std::move(leftBlock));
  generators2.push_back(std::move(rightBlock));

  return std::make_unique<cw::Rotator>(
      std::make_unique<cw::Composer>(std::move(generators2)),
      cw::constants::g_ZeroVertex,
      cw::Axis::XAxis,
      90.0
  );
}

} // namespace wgc0310
