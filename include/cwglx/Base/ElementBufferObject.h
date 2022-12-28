#ifndef PROJECT_GL2_ELEMENT_BUFFER_OBJECT_H
#define PROJECT_GL2_ELEMENT_BUFFER_OBJECT_H

#include "cwglx/GL/GL.h"
#include "util/Derive.h"

namespace cw {

class ElementBufferObject final {
public:
  explicit ElementBufferObject(GLFunctions *f);
  ~ElementBufferObject();

  void Bind(GLFunctions *f) const noexcept;
  void Unbind(GLFunctions *f) const noexcept;

  void BufferData(GLFunctions *f,
                  const GLuint *data,
                  std::size_t size,
                  GLenum drawHint = GL_STATIC_DRAW) const noexcept;

  void Delete(GLFunctions *f);

  CW_DERIVE_UNCOPYABLE(ElementBufferObject)
  CW_DERIVE_UNMOVABLE(ElementBufferObject)

private:
  GLuint m_EBO;
  bool m_Deleted;
};

} // namespace cw

#endif // PROJECT_GL2_ELEMENT_BUFFER_OBJECT_H
