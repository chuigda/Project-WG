#include "cwglx/Material.h"

#include "cwglx/GLImpl.h"

namespace cw {

Material::~Material() = default;

StandardMaterial::StandardMaterial(RGBAColor ambient,
                                   RGBAColor diffuse,
                                   RGBAColor specular,
                                   float shininess,
                                   GLenum colorDimension) noexcept
  : ambient(ambient),
    diffuse(diffuse),
    specular(specular),
    shininess(shininess),
    m_ColorDimension(colorDimension)
{}

StandardMaterial::StandardMaterial(const RGBAColorF &ambient,
                                   const RGBAColorF &diffuse,
                                   const RGBAColorF &specular,
                                   float shininess,
                                   GLenum colorDimension) noexcept
  : ambient(ambient),
    diffuse(diffuse),
    specular(specular),
    shininess(shininess),
    m_ColorDimension(colorDimension)
{}

StandardMaterial::~StandardMaterial() = default;

bool StandardMaterial::IsTransparent() const noexcept {
  return ambient.IsTransparent()
         || diffuse.IsTransparent()
         || specular.IsTransparent();
}

void StandardMaterial::Apply(GLFunctions *f) const noexcept {
  f->glColorMaterial(GL_FRONT, m_ColorDimension);
  f->glMaterialfv(GL_FRONT, GL_AMBIENT, ambient.GetRepr().data());
  f->glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse.GetRepr().data());
  f->glMaterialfv(GL_FRONT, GL_SPECULAR, specular.GetRepr().data());
  f->glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

#define CW_DEFINE_MATERIAL(NAME, \
                           AR, AG, AB, AA, \
                           DR, DG, DB, DA, \
                           SR, SG, SB, SA, \
                           SHINE) \
  const StandardMaterial* Get##NAME##Material() noexcept { \
    static const StandardMaterial material { \
      cw::RGBAColorF(AR, AG, AB, AA), \
      cw::RGBAColorF(DR, DG, DB, DA), \
      cw::RGBAColorF(SR, SG, SB, SA), \
      SHINE \
    }; \
    return &material; \
  }
#include "cwglx/DefaultMaterials.inc"

} // namespace cw
