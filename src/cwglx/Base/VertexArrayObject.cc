#include "include/cwglx/Base/VertexArrayObject.h"
#include "include/cwglx/GL/GLImpl.h"

namespace cw {
VertexArrayObject::VertexArrayObject(GLFunctions *f)
  : m_VAO(0),
    m_Deleted(false)
{
  f->glGenVertexArrays(1, &m_VAO);
}

VertexArrayObject::VertexArrayObject(GLuint vao, Sinkrate)
  : m_VAO(vao),
    m_Deleted(false)
{}

VertexArrayObject::~VertexArrayObject() {
  if (!m_Deleted) {
    qWarning() << "VertexArrayObject::~VertexArrayObject():"
               << "vertex array object deleted before releasing relevant OpenGL resources";
  }
}

void VertexArrayObject::Bind(GLFunctions *f) const {
  if (m_Deleted) {
    qWarning() << "VertexArrayObject::Bind(GLFunctions*):"
               << "cannot bind an already deleted vertex array object";
    return;
  }
  f->glBindVertexArray(m_VAO);
}

void VertexArrayObject::Unbind(GLFunctions *f) const {
  Q_UNUSED(this)

  f->glBindVertexArray(0);
}

void VertexArrayObject::Delete(GLFunctions *f) {
  if (m_Deleted) {
    return;
  }

  f->glDeleteVertexArrays(1, &m_VAO);
  m_Deleted = true;
}

std::vector<std::unique_ptr<VertexArrayObject>>
VertexArrayObject::CreateMany(GLFunctions *f, GLsizei count) {
  std::vector<std::unique_ptr<VertexArrayObject>> ret{};
  ret.reserve(count);

  std::vector<GLuint> vaoIds(count, 0);
  f->glGenVertexArrays(count, vaoIds.data());

  for (auto &vaoId : vaoIds) {
    ret.push_back(std::make_unique<VertexArrayObject>(
      vaoId,
      SecretInternalsDoNotUseOrYouWillBeFired
    ));
  }

  return ret;
}

} // namespace cw
