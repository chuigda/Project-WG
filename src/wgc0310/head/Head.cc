#include "wgc0310/head/Head.h"

#include "wgc0310/head/HeadFrame.h"
#include "wgc0310/head/Intake.h"
#include "wgc0310/head/ScreenGlass.h"

namespace wgc0310 {

std::unique_ptr<cw::TriangleGen> Head() {
  std::unique_ptr<cw::TriangleGen> frame = HeadFrame();
  std::unique_ptr<cw::TriangleGen> intake = IntakeRight();
  std::unique_ptr<cw::TriangleGen> intakeRight =
      std::make_unique<cw::Positioner>(std::move(intake),
                                       cw::Vector(11.0, 0.0, 0.0));
  std::unique_ptr<cw::TriangleGen> intakeLeft =
      std::make_unique<cw::Flipper>(intakeRight->Clone(),
                                    cw::Plane::YZPlane);

  std::vector<std::unique_ptr<cw::TriangleGen>> generators;
  generators.push_back(std::move(frame));
  generators.push_back(std::move(intakeRight));
  generators.push_back(std::move(intakeLeft));

  return std::make_unique<cw::Composer>(std::move(generators));
}

} // namespace wgc0310
