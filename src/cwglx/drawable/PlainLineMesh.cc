#include "cwglx/drawable/PlainLineMesh.h"
#include "cwglx/GLImpl.h"
#include <QDebug>

namespace cw {

PlainLineMesh::PlainLineMesh()
  : m_Vertices(),
    m_VBOInitialized(false),
    m_VBODeleted(false),
    m_VBO(0)
{}

PlainLineMesh::PlainLineMesh(const std::vector<Vertex> &vertices)
  : m_VBOInitialized(false),
    m_VBODeleted(false),
    m_VBO(0)
{
  m_Vertices.reserve(vertices.size());
  for (const Vertex &v : vertices) {
    m_Vertices.push_back(VertexF::Downscale(v));
  }
}

PlainLineMesh::~PlainLineMesh() {
  if (!m_VBODeleted) {
    qWarning() << "PlainLineMesh::~PlainLineMesh():"
               << "VBO not deleted";
  }
}

void PlainLineMesh::Draw(GLFunctions *f) const noexcept {
  if (m_VBODeleted) {
    qWarning() << "PlainLineMesh::Draw():"
               << "VBO deleted, skipping draw";
    return;
  }

  PreInitialize(f);

  f->glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
  {
    f->glEnableClientState(GL_VERTEX_ARRAY);
    f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    f->glVertexPointer(2, GL_FLOAT, 0, nullptr);

    f->glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_Vertices.size()));
  }
  f->glPopClientAttrib();
}

void PlainLineMesh::Delete(GLFunctions *f) const noexcept {
  if (m_VBODeleted) {
    qWarning() << "PlainLineMesh::Delete():"
               << "VBO already deleted";
    return;
  }

  f->glDeleteBuffers(1, &m_VBO);
  m_VBODeleted = true;
}

void PlainLineMesh::AddLine(const Vertex& start, const Vertex& end) {
  m_Vertices.push_back(VertexF::Downscale(start));
  m_Vertices.push_back(VertexF::Downscale(end));
}

void PlainLineMesh::AddLines(const Vertex *vertices,
                             std::size_t count) {
  for (std::size_t i = 0; i < count; i += 2) {
    m_Vertices.push_back(VertexF::Downscale(vertices[i]));
    m_Vertices.push_back(VertexF::Downscale(vertices[i + 1]));
  }
}

void PlainLineMesh::PreInitialize(GLFunctions *f) const noexcept{
  if (m_VBOInitialized) {
    qWarning() << "PlainLineMesh::PreInitialize():"
               << "VBO already initialized, skipping";
    return;
  }

  f->glGenBuffers(1, &m_VBO);
  f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  f->glBufferData(GL_ARRAY_BUFFER,
                  m_Vertices.size() * sizeof(VertexF),
                  m_Vertices.data(),
                  GL_STATIC_DRAW);
  f->glBindBuffer(GL_ARRAY_BUFFER, 0);

  m_VBOInitialized = true;
}

} // namespace cw
