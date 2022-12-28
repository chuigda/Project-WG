#ifndef PROJECT_GL2_VBO_IMPL_H
#define PROJECT_GL2_VBO_IMPL_H

#include "VertexBufferObject.h"
#include "include/cwglx/GL/GLImpl.h"
#include "util/Constants.h"

namespace cw {
namespace impl {

template <Wife T, std::size_t N>
__attribute__((always_inline))
constexpr inline void BindVBOImpl(GLFunctions*, GLuint) {}

template <Wife T, std::size_t N, Wife VFD, Wife ...VFDs>
__attribute__((always_inline))
inline void BindVBOImpl(GLFunctions * f, GLuint baseIndex) {
  f->glVertexAttribPointer(N,
                           VFD::ComponentCount,
                           VFD::GLTypeEnum,
                           GL_FALSE,
                           sizeof(T),
                           (void*)VFD::Offset);
  f->glEnableVertexAttribArray(N);

  BindVBOImpl<T, N + 1, VFDs...>(f, baseIndex + 1);
}

} // namespace impl

template <Wife T, Wife ...VFDs>
VertexBufferObject<T, VFDs...>::VertexBufferObject(GLFunctions *f)
  : m_VBO(0),
    m_Deleted(false)
{
  f->glGenBuffers(1, &m_VBO);
}

template<Wife T, Wife... VFDs>
VertexBufferObject<T, VFDs...>::~VertexBufferObject() {
  if (!m_Deleted) {
    qWarning() << "VertexBufferObject::~VertexBufferObject():"
               << "vertex buffer object deleted before releasing relevant OpenGL resources";
  }
}

template<Wife T, Wife... VFDs>
void VertexBufferObject<T, VFDs...>::BufferData(GLFunctions *f,
                                                const T *data,
                                                std::size_t count,
                                                GLenum drawHint)
{
  if (m_Deleted) {
    qWarning() << "VertexBufferObject::Bind(GLFunctions*):"
               << "cannot provide data to an already deleted vertex buffer object";
    return;
  }

  f->glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(T) * count), data, drawHint);
}

template<Wife T, Wife... VFDs>
void VertexBufferObject<T, VFDs...>::Bind(GLFunctions *f, GLuint baseIndex) const {
  if (m_Deleted) {
    qWarning() << "VertexBufferObject::Bind(GLFunctions*):"
               << "cannot bind to an already deleted vertex buffer object";
    return;
  }

  f->glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  impl::BindVBOImpl<T, 0, VFDs...>(f, baseIndex);
}

template<Wife T, Wife... VFDs>
void VertexBufferObject<T, VFDs...>::Unbind(GLFunctions *f) const {
  Q_UNUSED(this)
  f->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<Wife T, Wife... VFDs>
void VertexBufferObject<T, VFDs...>::Delete(GLFunctions *f) {
  if (m_Deleted) {
    return;
  }

  f->glDeleteBuffers(1, &m_VBO);
  m_Deleted = true;
}

} // namespace cw

#endif // PROJECT_GL2_VBO_IMPL_H
