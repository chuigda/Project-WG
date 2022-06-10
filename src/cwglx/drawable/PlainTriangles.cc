#include "cwglx/drawable/PlainTriangles.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

TriangleGenerator::~TriangleGenerator() = default;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
PlainTriangles::PlainTriangles(const std::vector<Vertex>& vertices,
                               bool computeNormal)
  : m_VBOInitialized(false),
    m_VBODeleted(false)
{
  m_Vertices.reserve(vertices.size());
  for (const auto& vertex : vertices) {
    m_Vertices.push_back(VertexF::Downscale(vertex));
  }

  m_Indices.reserve(vertices.size());
  for (std::size_t i = 0; i < vertices.size(); i++) {
    m_Indices.push_back(i);
  }

  if (computeNormal) {
    m_NormalVectors.reserve(vertices.size());
    for (std::size_t i = 0; i < vertices.size(); i += 3) {
      const Vertex &v0 = vertices[m_Indices[i]];
      const Vertex &v1 = vertices[m_Indices[i + 1]];
      const Vertex &v2 = vertices[m_Indices[i + 2]];

      const Vector v0v1 = v1 - v0;
      const Vector v1v2 = v2 - v1;

      const VectorF normal = VectorF::Downscale(v0v1 * v1v2);

      for (std::size_t j = 0; j < 3; j++) {
        m_NormalVectors.push_back(normal);
      }
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
    f->glGenBuffers(3, m_VBO.data());

    f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
    f->glBufferData(GL_ARRAY_BUFFER,
                    static_cast<GLsizei>(m_Vertices.size() * sizeof(VertexF)),
                    m_Vertices.data(),
                    GL_STATIC_DRAW);

    f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
    f->glBufferData(GL_ARRAY_BUFFER,
                    static_cast<GLsizei>(m_NormalVectors.size()
                                         * sizeof(VectorF)),
                    m_NormalVectors.data(),
                    GL_STATIC_DRAW);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[2]);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    static_cast<GLsizei>(m_Indices.size() * sizeof(GLushort)),
                    m_Indices.data(),
                    GL_STATIC_DRAW);

    m_VBOInitialized = true;
  }

  f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
  {
    f->glEnableClientState(GL_VERTEX_ARRAY);
    f->glEnableClientState(GL_NORMAL_ARRAY);

    f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
    f->glVertexPointer(3, GL_FLOAT, 0, nullptr);

    f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
    f->glNormalPointer(GL_FLOAT, 0, nullptr);

    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBO[2]);
    f->glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_SHORT, nullptr);
  }
  f->glPopClientAttrib();
}

void PlainTriangles::Delete(QOpenGLFunctions_2_0 *f) const noexcept {
  if (!m_VBODeleted) {
    f->glDeleteBuffers(2, m_VBO.data());
    m_VBODeleted = true;
  }
}

void PlainTriangles::AddTriangle(const std::array<Vertex, 3> &triangle) {
  if (m_VBOInitialized) {
    qWarning() << "PlainTriangles::AddTriangles():"
               << "VBO already initialized, any newly added triangles simply"
               << "wont draw!";
    return;
  }

  if (m_VBODeleted) {
    qWarning() << "PlainTriangles::AddTriangles():"
               << "VBO deleted, any newly added triangles simply wont draw!";
    return;
  }

  SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED(triangle);
}

void PlainTriangles::AddTriangles(TriangleGenerator *generator) {
  if (m_VBOInitialized) {
    qWarning() << "PlainTriangles::AddTriangles():"
               << "VBO already initialized, any newly added triangles simply"
               << "wont draw!";
    return;
  }

  if (m_VBODeleted) {
    qWarning() << "PlainTriangles::AddTriangles():"
               << "VBO deleted, any newly added triangles simply wont draw!";
    return;
  }

  while (generator->HasNextTriangle()) {
    const auto triangle = generator->NextTriangle();
    SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED(triangle);
  }
}

void PlainTriangles::SECRET_INTERNALS_DO_NOT_USE_OR_YOU_WILL_BE_FIRED(
    const std::array<Vertex, 3> &triangle
) {
  const auto &[v0, v1, v2] = triangle;

  const Vector v0v1 = v1 - v0;
  const Vector v1v2 = v2 - v1;
  const VectorF normal = VectorF::Downscale(v0v1 * v1v2);

  for (std::size_t i = 0; i < 3; i++) {
    m_Vertices.push_back(VertexF::Downscale(triangle[i]));
    m_NormalVectors.push_back(normal);
  }
}

#pragma clang diagnostic pop

} // namespace cws
