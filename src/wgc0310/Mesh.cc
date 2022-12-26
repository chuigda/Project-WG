#include "wgc0310/Mesh.h"

#include "cwglx/Drawable.h"
#include "cwglx/MeshLoader.h"

namespace wgc0310 {

WGCMeshCollection::WGCMeshCollection()
  : monitor(nullptr),
    monitorIntake(nullptr),
    wheel(nullptr),
    chestBox(nullptr),
    chestPlate(nullptr),
    power(nullptr),
    powerPin(nullptr),
    colorTimer(nullptr),
    colorTimerShell(nullptr),
    abdomen(nullptr),
    waist(nullptr),
    shoulder(nullptr),
    shoulderPlate(nullptr),
    bigArm(nullptr),
    bigArmCover(nullptr),
    bigArmConnector(nullptr),
    smallArm(nullptr),
    smallArmCover(nullptr),
    wheelSmall(nullptr),
    claw(nullptr),
    clawCover(nullptr)
{}

WGCMeshCollection::WGCMeshCollection(GLFunctions *f, cw::DrawableArena &arena)
  : WGCMeshCollection()
{
  Load(f, arena);
}

void WGCMeshCollection::Load(GLFunctions *f, cw::DrawableArena &arena) {
  #define LOAD_MESH_MTL(PATH, MTL, TGT) \
    { \
      std::unique_ptr<cw::PlainMesh> triangles = cw::LoadMesh(PATH); \
      triangles->PreInitialize(f); \
      const auto meshPtr = arena.Put(std::move(triangles)); \
      const auto mtlMeshPtr = arena.Put(                          \
        std::make_unique<cw::MaterializedDrawable>( \
          MTL, std::vector { meshPtr } \
        ) \
      ); \
      TGT = mtlMeshPtr; \
    }

  #define LOAD_MESH(PATH, TGT) \
    { \
      std::unique_ptr<cw::PlainMesh> mesh = cw::LoadMesh(PATH); \
      mesh->PreInitialize(f); \
      const auto meshPtr = arena.Put(std::move(mesh)); \
      TGT = meshPtr; \
    }

  #define LOAD_LINE_MESH(PATH, TGT) \
    {                               \
      std::unique_ptr<cw::PlainLineMesh> lineMesh = cw::LoadLineMesh(PATH); \
      lineMesh->PreInitialize(f);   \
      const auto meshPtr = arena.Put(std::move(lineMesh)); \
      TGT = meshPtr; \
    }

  const auto plastic = cw::GetPlasticMaterial();
  const auto chrome = cw::GetChromeMaterial();
  const auto steel = cw::GetSteel15N20Material();
  const auto brass = cw::GetBrassMaterial();
  const auto blackPlastic = cw::GetBlackPlasticMaterial();
  const auto glass = cw::GetGlassMaterial();

  LOAD_MESH_MTL("./model/wheel.mesh", blackPlastic, wheel)
  LOAD_MESH_MTL("./model/monitor.mesh", plastic, monitor)
  LOAD_MESH_MTL("./model/monitor-intake.mesh", chrome, monitorIntake)
  LOAD_LINE_MESH("./model/line-mesh/monitor.mesh", monitorStroke)

  LOAD_MESH_MTL("./model/chest-box.mesh", plastic, chestBox)
  LOAD_LINE_MESH("./model/line-mesh/chest-box.mesh", chestBoxStroke)
  LOAD_MESH_MTL("./model/chest-plate.mesh", chrome, chestPlate)
  LOAD_MESH_MTL("./model/power.mesh", blackPlastic, power)
  LOAD_MESH_MTL("./model/power-pin.mesh", brass, powerPin)
  LOAD_MESH_MTL("./model/ber-shell.mesh", glass, colorTimerShell)
  LOAD_MESH("./model/ber.mesh", colorTimer)

  LOAD_MESH_MTL("./model/abdomen.mesh", blackPlastic, abdomen)
  LOAD_MESH_MTL("./model/waist.mesh", plastic, waist)

  LOAD_MESH_MTL("./model/shoulder-connector.mesh", plastic, shoulder)
  LOAD_MESH_MTL("./model/shoulder-plate.mesh", chrome, shoulderPlate)

  LOAD_MESH_MTL("./model/big-arm.mesh", chrome, bigArm)
  LOAD_MESH_MTL("./model/big-arm-cover.mesh", plastic, bigArmCover)
  LOAD_MESH_MTL("./model/big-arm-connector.mesh", steel, bigArmConnector)
  LOAD_MESH_MTL("./model/small-arm.mesh", chrome, smallArm)
  LOAD_MESH_MTL("./model/small-arm-cover.mesh", plastic, smallArmCover)
  LOAD_MESH_MTL("./model/wheel-small.mesh", blackPlastic, wheelSmall)
  LOAD_MESH_MTL("./model/claw.mesh", steel, claw)
  LOAD_MESH_MTL("./model/claw-cover.mesh", plastic, clawCover)

  #undef LOAD_MESH_MTL
}

} // namespace wgc0310
