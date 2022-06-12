#ifndef PROJECT_WG_PLAIN_TRIANGLES_H
#define PROJECT_WG_PLAIN_TRIANGLES_H

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/Vertex.h"
#include "util/Derive.h"

namespace cw {

class TriangleGenerator;

class PlainTriangles final : public Drawable {
public:
  explicit PlainTriangles(bool computeNormal = true);

  explicit PlainTriangles(const std::vector<Vertex>& vertices,
                          bool computeNormal = true);

  ~PlainTriangles() final;

  void Draw(QOpenGLFunctions_2_0 *f) const noexcept final;

  void Delete(QOpenGLFunctions_2_0 *f) const noexcept final;

  CW_DERIVE_UNCOPYABLE(PlainTriangles)
  CW_DERIVE_UNMOVABLE(PlainTriangles)

  void AddTriangle(const std::array<Vertex, 3>& triangle);
  void AddTriangles(const std::array<Vertex, 3> *triangles, std::size_t count);
  void AddTriangles(TriangleGenerator *generator);

private:
  void SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED(
      const std::array<Vertex, 3> &triangle
  );

  bool m_ComputeNormal;
  std::vector<VertexF> m_Vertices;
  std::vector<GLuint> m_Indices;
  std::vector<VectorF> m_NormalVectors;

  mutable bool m_VBOInitialized;
  mutable bool m_VBODeleted;
  mutable std::array<GLuint, 3> m_VBO;
};

} // namespace cw

#endif // PROJECT_WG_PLAIN_TRIANGLES_H
