#ifndef PROJECT_WG_WGC0310_MESH_H
#define PROJECT_WG_WGC0310_MESH_H

#include <cstdint>
#include <memory>

#include "cwglx/GL/GL.h"
#include "cwglx/Object/Vertex.h"
#include "cwglx/Base/VertexArrayObject.h"
#include "cwglx/Base/ShaderProgram.h"

namespace wgc0310 {

class SimpleObject final {
public:
  SimpleObject(GLFunctions *f, cw::SimpleVertex *vertices, std::size_t vertexCount);

  void Draw(GLFunctions *f, bool unbind = false);

  void Delete(GLFunctions *f);

private:
  cw::VertexArrayObject m_VAO;
  cw::SimpleVertexVBO m_VBO;
  GLsizei m_VertexCount;
};

class StrokeObject final {
public:
  StrokeObject(GLFunctions *f, cw::PlainVertex *vertices, std::size_t vertexCount);

  void Draw(GLFunctions *f, bool unbind = false);

  void Delete(GLFunctions *f);

private:
  cw::VertexArrayObject m_VAO;
  cw::PlainVertexVBO m_VBO;
  GLsizei m_VertexCount;
};

struct WGCMeshCollection {
  // 调试用的茶壶
  SimpleObject teapot;

  // 头部模型
  SimpleObject monitor;
  SimpleObject monitorIntake;
  SimpleObject wheel;

  // 胸部模型
  SimpleObject chestBox;
  SimpleObject chestPlate;
  SimpleObject power;
  SimpleObject powerPin;
  SimpleObject colorTimer;
  SimpleObject colorTimerShell;

  // 腰部模型
  SimpleObject abdomen;
  SimpleObject waist;

  // 肩部模型
  SimpleObject shoulder;
  SimpleObject shoulderPlate;

  // 手臂模型
  SimpleObject bigArm;
  SimpleObject bigArmCover;
  SimpleObject bigArmConnector;

  SimpleObject smallArm;
  SimpleObject smallArmCover;

  SimpleObject claw;
  SimpleObject clawCover;
  SimpleObject wheelSmall;

  void Delete(GLFunctions *f);
};

std::unique_ptr<WGCMeshCollection> LoadWGCMesh(GLFunctions *f);

} // namespace wgc0310

#endif /* PROJECT_WG_WGC0310_MESH_H */
