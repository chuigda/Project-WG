#ifndef PROJECT_GL2_VERTEX_H
#define PROJECT_GL2_VERTEX_H

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "cwglx/Base/VertexBufferObject.h"
#include "cwglx/Base/VBOImpl/GLM.h"

namespace cw {

struct PlainVertex {
  glm::vec3 vertexCoord;
};

struct SimpleVertex {
  glm::vec3 vertexCoord;
  glm::vec3 vertexNormal;
};

struct Vertex {
  glm::vec3 vertexCoord;
  glm::vec3 vertexNormal;
  glm::vec4 color;
  glm::vec2 texCoord;
};

using PlainVertexVBO = CW_DEFINE_VBO_TYPE(
  PlainVertex,
  vertexCoord
);

using SimpleVertexVBO = CW_DEFINE_VBO_TYPE(
  SimpleVertex,
  vertexCoord,
  vertexNormal
);

using VertexVBO = CW_DEFINE_VBO_TYPE(
  Vertex,
  vertexCoord,
  vertexNormal,
  color,
  texCoord
);

} // namespace cw

#endif // PROJECT_GL2_VERTEX_H
