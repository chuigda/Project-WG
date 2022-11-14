#include "wgc0310/Mesh.h"

#include "cwglx/Drawable.h"
#include "cwglx/MeshLoader.h"

namespace wgc0310 {

WGCMeshCollection::WGCMeshCollection(GLFunctions *f, cw::DrawableArena &arena) {
  #define LOAD_MESH_MTL(PATH, MTL, TGT) \
    { \
      std::unique_ptr<cw::PlainTriangles> triangles = cw::LoadMesh(PATH); \
      triangles->PreInitialize(f); \
      const auto [_, meshPtr] = arena.Put(std::move(triangles)); \
      const auto [_2, mtlMeshPtr] = arena.Put(                          \
        std::make_unique<cw::MaterializedDrawable>( \
          MTL, std::vector { meshPtr } \
        ) \
      ); \
      TGT = mtlMeshPtr; \
    }

  #define LOAD_MESH(PATH, TGT) \
    {                        \
      std::unique_ptr<cw::PlainTriangles> triangles = cw::LoadMesh(PATH); \
      triangles->PreInitialize(f); \
      const auto [_, meshPtr] = arena.Put(std::move(triangles)); \
      TGT = meshPtr; \
    }

  const auto plastic = cw::GetPlasticMaterial();
  const auto chrome = cw::GetChromeMaterial();
  const auto steel = cw::GetSteel15N20Material();
  const auto brass = cw::GetBrassMaterial();
  const auto blackPlastic = cw::GetBlackPlasticMaterial();
  const auto glass = cw::GetGlassMaterial();

  LOAD_MESH_MTL("./resc/model/wheel.mesh", blackPlastic, wheel);
  LOAD_MESH_MTL("./resc/model/monitor.mesh", plastic, monitor)
  LOAD_MESH_MTL("./resc/model/monitor-intake.mesh", chrome, monitorIntake)
  LOAD_MESH_MTL("./resc/model/radar.mesh", brass, radar)

  LOAD_MESH_MTL("./resc/model/chest-box.mesh", plastic, chestBox)
  LOAD_MESH_MTL("./resc/model/chest-plate.mesh", chrome, chestPlate)
  LOAD_MESH_MTL("./resc/model/power.mesh", blackPlastic, power)
  LOAD_MESH_MTL("./resc/model/power-pin.mesh", brass, powerPin)
  LOAD_MESH_MTL("./resc/model/ber-shell.mesh", glass, berShell)
  LOAD_MESH("./resc/model/ber.mesh", ber)

  LOAD_MESH_MTL("./resc/model/shoulder-connector.mesh", plastic, shoulder)
  LOAD_MESH_MTL("./resc/model/shoulder-plate.mesh", chrome, shoulderPlate)

  LOAD_MESH_MTL("./resc/model/big-arm.mesh", chrome, bigArm)
  LOAD_MESH_MTL("./resc/model/big-arm-cover.mesh", plastic, bigArmCover)
  LOAD_MESH_MTL("./resc/model/big-arm-connector.mesh", steel, bigArmConnector)
  LOAD_MESH_MTL("./resc/model/small-arm.mesh", chrome, smallArm)
  LOAD_MESH_MTL("./resc/model/small-arm-cover.mesh", plastic, smallArmCover)
  LOAD_MESH_MTL("./resc/model/wheel-small.mesh", blackPlastic, wheelSmall)
  LOAD_MESH_MTL("./resc/model/claw.mesh", steel, claw)
  LOAD_MESH_MTL("./resc/model/claw-cover.mesh", plastic, clawCover)

  #undef LOAD_MESH_MTL
}

} // namespace wgc0310
