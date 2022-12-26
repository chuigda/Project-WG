#ifndef PROJECT_WG_PLAIN_MESH_H
#define PROJECT_WG_PLAIN_MESH_H

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/Vertex.h"
#include "util/Derive.h"
#include "util/Sinkrate.h"

namespace cw {

class PlainMesh final : public Drawable {
public:
  explicit PlainMesh(bool computeNormal = true);

  explicit PlainMesh(const std::vector<Vertex>& vertices,
                     bool computeNormal = true);

  ~PlainMesh() final;

  void Draw(GLFunctions *f) const noexcept final;

  void Delete(GLFunctions *f) const noexcept final;

  CW_DERIVE_UNCOPYABLE(PlainMesh)
  CW_DERIVE_UNMOVABLE(PlainMesh)

  void AddTriangle(const Triangle& triangle);
  void AddTriangles(const Triangle *triangles, std::size_t count);

  void PreInitialize(GLFunctions *f) const;

private:
  void AddTriangleImpl(const Triangle &triangle);

  bool m_ComputeNormal;
  std::vector<VertexF> m_Vertices;
  std::vector<VectorF> m_NormalVectors;

  mutable bool m_VBOInitialized;
  mutable bool m_VBODeleted;
  mutable std::array<GLuint, 2> m_VBO;
};

} // namespace cw

#endif // PROJECT_WG_PLAIN_MESH_H
