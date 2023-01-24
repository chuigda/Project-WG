#include "wgc0310/Mesh.h"

#include "cwglx/GL/GLImpl.h"
#include "cwglx/Base/Shader.h"
#include "cwglx/Object/WavefrontLoader.h"
#include "util/FileUtil.h"

namespace wgc0310 {

SimpleObject::SimpleObject(GLFunctions *f, cw::SimpleVertex *vertices, std::size_t vertexCount)
  : m_VAO(f),
    m_VBO(f),
    m_VertexCount(static_cast<GLsizei>(vertexCount))
{
  m_VAO.Bind(f);
  m_VBO.Bind(f);
  m_VBO.BufferData(f, vertices, vertexCount);

  m_VAO.Unbind(f);
  m_VBO.Unbind(f);
}

void SimpleObject::Draw(GLFunctions *f, bool unbind) {
  m_VAO.Bind(f);
  f->glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
  if (unbind) {
    m_VAO.Unbind(f);
  }
}

void SimpleObject::Delete(GLFunctions *f) {
  m_VAO.Delete(f);
  m_VBO.Delete(f);
}

StrokeObject::StrokeObject(GLFunctions *f, cw::PlainVertex *vertices, std::size_t vertexCount)
  : m_VAO(f),
    m_VBO(f),
    m_VertexCount(static_cast<GLsizei>(vertexCount))
{
  m_VAO.Bind(f);
  m_VBO.Bind(f);
  m_VBO.BufferData(f, vertices, vertexCount);

  m_VAO.Unbind(f);
  m_VBO.Unbind(f);
}

void StrokeObject::Draw(GLFunctions *f, bool unbind) {
  m_VAO.Bind(f);
  f->glDrawArrays(GL_LINES, 0, m_VertexCount);
  if (unbind) {
    m_VAO.Unbind(f);
  }
}

void StrokeObject::Delete(GLFunctions *f) {
  m_VAO.Delete(f);
  m_VBO.Delete(f);
}

std::unique_ptr<WGCMeshCollection> LoadWGCMesh(GLFunctions *f) {
  auto teapot = cw::LoadMesh("./model/teapot.mesh", 6.0);

  auto monitor = cw::LoadMesh("./model/monitor.mesh");
  auto monitorIntake = cw::LoadMesh("./model/monitor-intake.mesh");
  auto wheel = cw::LoadMesh("./model/wheel.mesh");

  auto chestBox = cw::LoadMesh("./model/chest-box.mesh");
  auto chestPlate = cw::LoadMesh("./model/chest-plate.mesh");
  auto power = cw::LoadMesh("./model/power.mesh");
  auto powerPin = cw::LoadMesh("./model/power-pin.mesh");
  auto colorTimer = cw::LoadMesh("./model/ber.mesh");
  auto colorTimerShell = cw::LoadMesh("./model/ber-shell.mesh");

  auto abdomen = cw::LoadMesh("./model/abdomen.mesh");
  auto waist = cw::LoadMesh("./model/waist.mesh");

  auto shoulder = cw::LoadMesh("./model/shoulder-connector.mesh");
  auto shoulderPlate = cw::LoadMesh("./model/shoulder-plate.mesh");

  auto bigArm = cw::LoadMesh("./model/big-arm.mesh");
  auto bigArmCover = cw::LoadMesh("./model/big-arm-cover.mesh");
  auto bigArmConnector = cw::LoadMesh("./model/big-arm-connector.mesh");

  auto smallArm = cw::LoadMesh("./model/small-arm.mesh");
  auto smallArmCover = cw::LoadMesh("./model/small-arm-cover.mesh");

  auto claw = cw::LoadMesh("./model/claw.mesh");
  auto clawCover = cw::LoadMesh("./model/claw-cover.mesh");

  auto wheelSmall = cw::LoadMesh("./model/wheel-small.mesh");

  return std::unique_ptr<WGCMeshCollection>(new WGCMeshCollection { // NOLINT(modernize-make-unique)
    .teapot = SimpleObject(f, teapot.data(), teapot.size()),

    .monitor = SimpleObject(f, monitor.data(), monitor.size()),
    .monitorIntake = SimpleObject(f, monitorIntake.data(), monitorIntake.size()),
    .wheel = SimpleObject(f, wheel.data(), wheel.size()),

    .chestBox = SimpleObject(f, chestBox.data(), chestBox.size()),
    .chestPlate = SimpleObject(f, chestPlate.data(), chestPlate.size()),
    .power = SimpleObject(f, power.data(), power.size()),
    .powerPin = SimpleObject(f, powerPin.data(), powerPin.size()),
    .colorTimer = SimpleObject(f, colorTimer.data(), colorTimer.size()),
    .colorTimerShell = SimpleObject(f, colorTimerShell.data(), colorTimerShell.size()),

    .abdomen = SimpleObject(f, abdomen.data(), abdomen.size()),
    .waist = SimpleObject(f, waist.data(), waist.size()),

    .shoulder = SimpleObject(f, shoulder.data(), shoulder.size()),
    .shoulderPlate = SimpleObject(f, shoulderPlate.data(), shoulderPlate.size()),

    .bigArm = SimpleObject(f, bigArm.data(), bigArm.size()),
    .bigArmCover = SimpleObject(f, bigArmCover.data(), bigArmCover.size()),
    .bigArmConnector = SimpleObject(f, bigArmConnector.data(), bigArmConnector.size()),

    .smallArm = SimpleObject(f, smallArm.data(), smallArm.size()),
    .smallArmCover = SimpleObject(f, smallArmCover.data(), smallArmCover.size()),

    .claw = SimpleObject(f, claw.data(), claw.size()),
    .clawCover = SimpleObject(f, clawCover.data(), clawCover.size()),

    .wheelSmall = SimpleObject(f, wheelSmall.data(), wheelSmall.size())
  });
}

void WGCMeshCollection::Delete(GLFunctions *f) {
  monitor.Delete(f);
  monitorIntake.Delete(f);
}

} // namespace wgc0310
