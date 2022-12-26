#ifndef PROJECT_WG_PLAIN_LINE_MESH_H
#define PROJECT_WG_PLAIN_LINE_MESH_H

#include <vector>
#include "cwglx/Vertex.h"
#include "cwglx/Drawable.h"
#include "util/Derive.h"

namespace cw {

class PlainLineMesh final : public Drawable {
public:
  explicit PlainLineMesh();

  explicit PlainLineMesh(const std::vector<Vertex>& vertices);

  ~PlainLineMesh() final;

  void Draw(GLFunctions *f) const noexcept final;

  void Delete(GLFunctions *f) const noexcept final;

  CW_DERIVE_UNCOPYABLE(PlainLineMesh)
  CW_DERIVE_UNMOVABLE(PlainLineMesh)

  void AddLine(const Vertex& start, const Vertex& end);

  void AddLines(const Vertex *vertices, std::size_t count);

  void PreInitialize(GLFunctions *f);

private:
  std::vector<VertexF> m_Vertices;

  mutable bool m_VBOInitialized;
  mutable bool m_VBODeleted;
  mutable GLuint m_VBO;
};

} // namespace cw

#endif // PROJECT_WG_PLAIN_LINE_MESH_H
