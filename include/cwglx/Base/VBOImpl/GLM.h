#ifndef PROJECT_GL2_GLM_H
#define PROJECT_GL2_GLM_H

#include <glm/fwd.hpp>
#include "include/cwglx/GL/GL.h"
#include "util/Wife.h"

namespace cw::impl {

template <Wife T> struct VFDProbe;

template<> struct VFDProbe<glm::vec1> {
  static constexpr GLenum GLTypeEnum = GL_FLOAT;
  static constexpr std::size_t ComponentCount = 1;
};

template<> struct VFDProbe<glm::vec2> {
  static constexpr GLenum GLTypeEnum = GL_FLOAT;
  static constexpr std::size_t ComponentCount = 2;
};

template<> struct VFDProbe<glm::vec3> {
  static constexpr GLenum GLTypeEnum = GL_FLOAT;
  static constexpr std::size_t ComponentCount = 3;
};

template<> struct VFDProbe<glm::vec4> {
  static constexpr GLenum GLTypeEnum = GL_FLOAT;
  static constexpr std::size_t ComponentCount = 4;
};

} // namespace cw::impl

#endif // PROJECT_GL2_GLM_H
