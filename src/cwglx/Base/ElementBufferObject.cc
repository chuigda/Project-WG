#include "cwglx/Base/ElementBufferObject.h"

#include "cwglx/GL/GLImpl.h"

namespace cw {

ElementBufferObject::ElementBufferObject(GLFunctions *f)
  : m_Deleted(false)
{
  f->glGenBuffers(1, &m_EBO);
}

ElementBufferObject::~ElementBufferObject() {
  if (!m_Deleted) {
    qWarning() << "ElementBufferObject::~ElementBufferObject():"
               << "element buffer object deleted before releasing relevant OpenGL resources";
  }
}

void ElementBufferObject::Bind(GLFunctions *f) const noexcept {
  if (m_Deleted) {
    qWarning() << "ElementBufferObject::Bind(GLFunctions*):"
               << "cannot bind to an already deleted element buffer object";
    return;
  }

  f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
}

void ElementBufferObject::Unbind(GLFunctions *f) const noexcept {
  Q_UNUSED(this)

  f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ElementBufferObject::BufferData(GLFunctions *f,
                                     const GLuint *data,
                                     std::size_t size,
                                     GLenum drawHint) const noexcept {
  if (m_Deleted) {
    qWarning() << "ElementBufferObject::Bind(GLFunctions*):"
               << "cannot provide data to an already deleted element buffer object";
    return;
  }

  f->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                  static_cast<GLsizeiptr>(size * sizeof(GLuint)),
                  data, drawHint);
}

void ElementBufferObject::Delete(GLFunctions *f) {
  if (m_Deleted) {
    return;
  }

  f->glDeleteBuffers(1, &m_EBO);
  m_Deleted = true;
}

} // namespace cw
