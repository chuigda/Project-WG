#ifndef PROJECT_GL2_VBO_H
#define PROJECT_GL2_VBO_H

#include <cstddef>
#include "include/cwglx/GL/GL.h"
#include "util/Wife.h"
#include "util/Derive.h"

namespace cw {

template <Wife T, Wife ...VFDs>
class VertexBufferObject {
public:
  explicit VertexBufferObject(GLFunctions *f);

  ~VertexBufferObject();

  void BufferData(GLFunctions *f,
                  T const* data,
                  std::size_t count,
                  GLenum drawHint = GL_STATIC_DRAW);

  void Bind(GLFunctions *f, GLuint baseIndex = 0) const;

  void Unbind(GLFunctions *f) const;

  void Delete(GLFunctions *f);

  CW_DERIVE_UNCOPYABLE(VertexBufferObject)
  CW_DERIVE_UNMOVABLE(VertexBufferObject)

private:
  GLuint m_VBO;
  bool m_Deleted;
};

namespace impl {
template <std::size_t Offset_, GLenum GLTypeEnum_, std::size_t ComponentCount_>
struct VertexFieldDescriptor {
  static constexpr GLenum GLTypeEnum = GLTypeEnum_;
  static constexpr std::size_t Offset = Offset_;
  static constexpr std::size_t ComponentCount = ComponentCount_;
};

template<Wife T> constexpr inline GLenum GetGLTypeEnum();

template<> constexpr inline GLenum GetGLTypeEnum<GLbyte>() { return GL_BYTE; }
template<> constexpr inline GLenum GetGLTypeEnum<GLubyte>() { return GL_UNSIGNED_BYTE; }
template<> constexpr inline GLenum GetGLTypeEnum<GLint>() { return GL_INT; }
template<> constexpr inline GLenum GetGLTypeEnum<GLuint>() { return GL_UNSIGNED_INT; }
template<> constexpr inline GLenum GetGLTypeEnum<GLfloat>() { return GL_FLOAT; }
template<> constexpr inline GLenum GetGLTypeEnum<GLdouble>() { return GL_DOUBLE; }

// GLboolean == GLubyte, no need of defining that again
// template<> constexpr inline GLenum GetGLTypeEnum<GLboolean>() { return GL_BOOL; }

template <Wife T>
struct VFDProbe {};

template <Wife T, std::size_t N>
struct VFDProbe<std::array<T, N>> {
  static constexpr GLenum GLTypeEnum = GetGLTypeEnum<T>();
  static constexpr std::size_t ComponentCount = N;
};

template <Wife T, std::size_t N>
struct VFDProbe<T[N]> {
  static constexpr GLenum GLTypeEnum = GetGLTypeEnum<T>();
  static constexpr std::size_t ComponentCount = N;
};

} // namespace impl
} // namespace cw

#define CW_IMPL_VFD_PROBE(T, FIELD) \
  cw::impl::VertexFieldDescriptor< \
    offsetof(T, FIELD), \
    cw::impl::VFDProbe<decltype(std::declval<T>().FIELD)>::GLTypeEnum, \
    cw::impl::VFDProbe<decltype(std::declval<T>().FIELD)>::ComponentCount \
  >

#define CW_IMPL_DEFINE_VBO_TYPE1(T, F1) \
  cw::VertexBufferObject< \
    T, \
    CW_IMPL_VFD_PROBE(T, F1) \
  >

#define CW_IMPL_DEFINE_VBO_TYPE2(T, F1, F2) \
  cw::VertexBufferObject< \
    T, \
    CW_IMPL_VFD_PROBE(T, F1), \
    CW_IMPL_VFD_PROBE(T, F2) \
  >

#define CW_IMPL_DEFINE_VBO_TYPE3(T, F1, F2, F3) \
  cw::VertexBufferObject< \
    T, \
    CW_IMPL_VFD_PROBE(T, F1), \
    CW_IMPL_VFD_PROBE(T, F2), \
    CW_IMPL_VFD_PROBE(T, F3) \
  >

#define CW_IMPL_DEFINE_VBO_TYPE4(T, F1, F2, F3, F4) \
  cw::VertexBufferObject< \
    T, \
    CW_IMPL_VFD_PROBE(T, F1), \
    CW_IMPL_VFD_PROBE(T, F2), \
    CW_IMPL_VFD_PROBE(T, F3), \
    CW_IMPL_VFD_PROBE(T, F4) \
  >

#define CW_IMPL_DEFINE_VBO_TYPE5(T, F1, F2, F3, F4, F5) \
  cw::VertexBufferObject< \
    T, \
    CW_IMPL_VFD_PROBE(T, F1), \
    CW_IMPL_VFD_PROBE(T, F2), \
    CW_IMPL_VFD_PROBE(T, F3), \
    CW_IMPL_VFD_PROBE(T, F4), \
    CW_IMPL_VFD_PROBE(T, F5), \
  >

#define CW_IMPL_SELECT_DEFINE_MACRO(_1, _2, _3, _4, _5, _6, NAME, ...) NAME

#define CW_DEFINE_VBO_TYPE(...) \
  CW_IMPL_SELECT_DEFINE_MACRO( \
    __VA_ARGS__, \
    CW_IMPL_DEFINE_VBO_TYPE5, \
    CW_IMPL_DEFINE_VBO_TYPE4, \
    CW_IMPL_DEFINE_VBO_TYPE3, \
    CW_IMPL_DEFINE_VBO_TYPE2, \
    CW_IMPL_DEFINE_VBO_TYPE1, \
  )(__VA_ARGS__)

#endif // PROJECT_GL2_VBO_H
