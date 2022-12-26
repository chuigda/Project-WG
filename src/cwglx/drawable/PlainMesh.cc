#include "cwglx/drawable/PlainMesh.h"
#include "cwglx/GLImpl.h"

namespace cw {

PlainMesh::PlainMesh(bool computeNormal)
  : m_ComputeNormal(computeNormal),
    m_Vertices(),
    m_NormalVectors(),
    m_VBOInitialized(false),
    m_VBODeleted(false),
    m_VBO()
{}

PlainMesh::PlainMesh(const std::vector<Vertex>& vertices,
                     bool computeNormal)
  : m_ComputeNormal(computeNormal),
    m_Vertices(),
    m_NormalVectors(),
    m_VBOInitialized(false),
    m_VBODeleted(false),
    m_VBO()
{
  m_Vertices.reserve(vertices.size());
  for (const auto& vertex : vertices) {
    m_Vertices.push_back(VertexF::Downscale(vertex));
  }

  if (computeNormal) {
    m_NormalVectors.reserve(vertices.size());
    for (std::size_t i = 0; i < vertices.size(); i += 3) {
      const Vertex &v0 = vertices[i];
      const Vertex &v1 = vertices[i + 1];
      const Vertex &v2 = vertices[i + 2];

      const Vector v0v1 = v1 - v0;
      const Vector v1v2 = v2 - v1;

      const VectorF normal = VectorF::Downscale(v0v1 * v1v2);

      for (std::size_t j = 0; j < 3; j++) {
        m_NormalVectors.push_back(normal);
      }
    }
  }
}

PlainMesh::~PlainMesh() {
  if (!m_VBODeleted) {
    qWarning() << "PlainMesh::~PlainMesh():"
               << "VBO not deleted";
  }
}

void PlainMesh::Draw(GLFunctions *f) const noexcept {
  if (m_VBODeleted) {
    qWarning() << "PlainMesh::Draw():"
               << "VBO deleted, skipping draw";
    return;
  }

  PreInitialize(f);

  f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
  {
    f->glEnableClientState(GL_VERTEX_ARRAY);
    f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
    f->glVertexPointer(3, GL_FLOAT, 0, nullptr);

    if (m_ComputeNormal) {
      f->glEnableClientState(GL_NORMAL_ARRAY);
      f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
      f->glNormalPointer(GL_FLOAT, 0, nullptr);
    }

    f->glDrawArrays(GL_TRIANGLES,
                    0,
                    static_cast<GLsizei>(m_Vertices.size()));
  }
  f->glPopClientAttrib();
}

void PlainMesh::Delete(GLFunctions *f) const noexcept {
  if (!m_VBODeleted) {
    f->glDeleteBuffers(2, m_VBO.data());
    m_VBODeleted = true;
  }
}

void PlainMesh::AddTriangle(const Triangle &triangle) {
  if (m_VBOInitialized) {
    qWarning() << "PlainMesh::AddTriangle():"
               << "VBO already initialized, any newly added triangles simply"
               << "wont draw!";
    return;
  }

  if (m_VBODeleted) {
    qWarning() << "PlainMesh::AddTriangle():"
               << "VBO deleted, any newly added triangles simply wont draw!";
    return;
  }

  AddTriangleImpl(triangle);
}

void PlainMesh::AddTriangles(const Triangle *triangles,
                             std::size_t count) {
  if (m_VBOInitialized) {
    qWarning() << "PlainMesh::AddTriangles():"
               << "VBO already initialized, any newly added triangles simply"
               << "wont draw!";
    return;
  }

  if (m_VBODeleted) {
    qWarning() << "PlainMesh::AddTriangles():"
               << "VBO deleted, any newly added triangles simply wont draw!";
    return;
  }

  for (std::size_t i = 0; i < count; i++) {
    const Triangle& triangle = triangles[i];
    AddTriangleImpl(triangle);
  }
}

void PlainMesh::AddTriangleImpl(const Triangle &triangle) {
  const auto &[v0, v1, v2] = triangle;

  const Vector v0v1 = v1 - v0;
  const Vector v1v2 = v2 - v1;
  const VectorF normal = VectorF::Downscale(v0v1 * v1v2);

  for (std::size_t i = 0; i < 3; i++) {
    m_Vertices.push_back(VertexF::Downscale(triangle[i]));
    if (m_ComputeNormal) {
      m_NormalVectors.push_back(normal);
    }
  }
}

void PlainMesh::PreInitialize(GLFunctions *f) const {
  if (!m_VBOInitialized) {
    qDebug() << "PlainMesh::PreInitialize():"
             << m_Vertices.size() / 3
             << "triangles ready to draw";

    f->glGenBuffers(2, m_VBO.data());

    f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
    f->glBufferData(GL_ARRAY_BUFFER,
                    static_cast<GLsizei>(m_Vertices.size() * sizeof(VertexF)),
                    m_Vertices.data(),
                    GL_STATIC_DRAW);

    if (m_ComputeNormal) {
      f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
      f->glBufferData(GL_ARRAY_BUFFER,
                      static_cast<GLsizei>(m_NormalVectors.size()
                                           * sizeof(VectorF)),
                      m_NormalVectors.data(),
                      GL_STATIC_DRAW);
    }

    m_VBOInitialized = true;
  }
}

#pragma clang diagnostic pop

} // namespace cws
