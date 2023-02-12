#ifndef PROJECT_GL2_MATERIAL_H
#define PROJECT_GL2_MATERIAL_H

#include <array>
#include <glm/vec4.hpp>
#include "cwglx/GL/GL.h"
#include "cwglx/Base/Texture.h"
#include "util/Sinkrate.h"

namespace cw {

class GLObjectContext;

struct Material {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  GLfloat shine;

  cw::Texture2D const* diffuseTexture;
  cw::Texture2D const* specularTexture;
  cw::Texture2D const* normalTexture;

  constexpr inline Material(glm::vec4 const& ambient,
                            glm::vec4 const& diffuse,
                            glm::vec4 const& specular,
                            GLfloat shine,
                            cw::Texture2D const* diffuseTexture,
                            cw::Texture2D const* specularTexture,
                            cw::Texture2D const* normalTexture)
    : ambient(ambient),
      diffuse(diffuse),
      specular(specular),
      shine(shine),
      diffuseTexture(diffuseTexture),
      specularTexture(specularTexture),
      normalTexture(normalTexture)
  {}

  constexpr inline Material() : Material(
    glm::vec4 { 0.0, 0.0, 0.0, 1.0 },
    glm::vec4 { 0.0, 0.0, 0.0, 1.0 },
    glm::vec4 { 0.0, 0.0, 0.0, 1.0 },
    0.0,
    nullptr,
    nullptr,
    nullptr
  ) {}

  CW_DERIVE_UNCOPYABLE(Material)
  CW_DERIVE_UNMOVABLE(Material)
};

} // namespace cw

#endif // PROJECT_GL2_MATERIAL_H
