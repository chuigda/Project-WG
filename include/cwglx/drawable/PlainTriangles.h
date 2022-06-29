#ifndef PROJECT_WG_PLAIN_TRIANGLES_H
#define PROJECT_WG_PLAIN_TRIANGLES_H

#include <vector>
#include "cwglx/Drawable.h"
#include "cwglx/Vertex.h"
#include "util/Derive.h"
#include "util/Sinkrate.h"

namespace cw {

class TriangleGen;

class FineTriangleGen;

class PlainTriangles final : public Drawable {
public:
  explicit PlainTriangles(bool computeNormal = true);

  explicit PlainTriangles(const std::vector<Vertex>& vertices,
                          bool computeNormal = true);

  ~PlainTriangles() final;

  void Draw(GLFunctions *f) const noexcept final;

  void Delete(GLFunctions *f) const noexcept final;

  CW_DERIVE_UNCOPYABLE(PlainTriangles)
  CW_DERIVE_UNMOVABLE(PlainTriangles)

  void AddTriangle(const Triangle& triangle);
  void AddTriangles(const Triangle *triangles, std::size_t count);
  void AddTriangles(TriangleGen *generator);
  void AddTriangles(FineTriangleGen *generator);

  void PreInitialize(GLFunctions *f) const;

private:
  void AddTriangleInner(const Triangle &triangle,
                        const SecretInternalsDoNotUseOrYouWillBeFired&);
  void AddTrianglesInner(const Triangle &triangle,
                         const TriangleNormal &normals,
                         const SecretInternalsDoNotUseOrYouWillBeFired&);

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
