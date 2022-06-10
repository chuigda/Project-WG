#ifndef PROJECT_WG_PLAIN_TRIANGLES_H
#define PROJECT_WG_PLAIN_TRIANGLES_H

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/Vertex.h"
#include "util/Derive.h"

namespace cw {

class TriangleGenerator {
public:
  virtual ~TriangleGenerator() = 0;

  virtual bool HasNextTriangle() = 0;
  virtual std::array<Vertex, 3> NextTriangle() = 0;
};

class PlainTriangles final : public Drawable {
public:
  explicit PlainTriangles(const std::vector<Vertex>& vertices,
                          bool computeNormal = true);

  ~PlainTriangles() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

  void Delete(QOpenGLFunctions_2_0 *f) const noexcept final;

  CW_DERIVE_UNCOPYABLE(PlainTriangles)
  CW_DERIVE_UNMOVABLE(PlainTriangles)

  void AddTriangles(TriangleGenerator *generator);

private:
  std::vector<VertexF> m_Vertices;
  std::vector<GLushort> m_Indices;
  std::vector<VectorF> m_NormalVectors;

  mutable bool m_VBOInitialized;
  mutable bool m_VBODeleted;
  mutable std::array<GLuint, 3> m_VBO;
};

} // namespace cw

#endif // PROJECT_WG_PLAIN_TRIANGLES_H
