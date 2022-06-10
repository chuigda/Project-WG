#include "cwglx/drawable/PlainTriangles.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
PlainTriangles::PlainTriangles(const std::vector<Vertex>& vertices,
                               std::vector<GLuint> &&indices,
                               bool computeNormal)
  : m_Indices(std::move(indices)),
    m_VBOInitialized(false),
    m_VBODeleted(false)
{
  m_Vertices.reserve(vertices.size());
  for (const auto& vertex : vertices) {
    m_Vertices.push_back(VertexF::Downscale(vertex));
  }

  if (computeNormal) {
    m_NormalVectors.reserve(m_Indices.size() / 3);
    for (std::size_t i = 0; i < m_Indices.size(); i += 3) {
      const auto &v0 = vertices[m_Indices[i]];
      const auto &v1 = vertices[m_Indices[i + 1]];
      const auto &v2 = vertices[m_Indices[i + 2]];

      const auto v0v1 = v1 - v0;
      const auto v1v2 = v2 - v1;

      const auto normal = v0v1 * v1v2;
      m_NormalVectors.push_back(VectorF::Downscale(normal));
    }
  }
}

PlainTriangles::~PlainTriangles() {
  if (!m_VBODeleted) {
    qWarning() << "PlainTriangles::~PlainTriangles():"
               << "VBO not deleted";
  }
}

void PlainTriangles::Draw(QOpenGLFunctions_2_0 *f) const noexcept {
  if (m_VBODeleted) {
    qWarning() << "PlainTriangles::Draw():"
               << "VBO deleted, skipping draw";
    return;
  }

  if (!m_VBOInitialized) {
    // TODO initialize VBO
  }

  // TODO draw with VBO
}

void PlainTriangles::Delete(QOpenGLFunctions_2_0 *f) const noexcept {
  if (!m_VBODeleted) {
    f->glDeleteBuffers(2, m_VBO.data());
  }
}

#pragma clang diagnostic pop

} // namespace cws
