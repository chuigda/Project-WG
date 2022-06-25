#include "cwglx/Material.h"

#include "cwglx/GLImpl.h"

namespace cw {

Material::~Material() = default;

StandardMaterial::StandardMaterial(RGBAColor ambient,
                                   RGBAColor diffuse,
                                   RGBAColor specular,
                                   float shininess,
                                   GLenum colorDimension) noexcept
  : m_Ambient(ambient),
    m_Diffuse(diffuse),
    m_Specular(specular),
    m_Shine(shininess),
    m_ColorDimension(colorDimension)
{}

StandardMaterial::StandardMaterial(const RGBAColorF &ambient,
                                   const RGBAColorF &diffuse,
                                   const RGBAColorF &specular,
                                   float shininess,
                                   GLenum colorDimension) noexcept
  : m_Ambient(ambient),
    m_Diffuse(diffuse),
    m_Specular(specular),
    m_Shine(shininess),
    m_ColorDimension(colorDimension)
{}

StandardMaterial::~StandardMaterial() = default;

bool StandardMaterial::IsTransparent() const noexcept {
  return m_Ambient.IsTransparent()
         || m_Diffuse.IsTransparent()
         || m_Specular.IsTransparent();
}

void StandardMaterial::Apply(GLFunctions *f) const noexcept {
  f->glColorMaterial(GL_FRONT, m_ColorDimension);
  f->glMaterialfv(GL_FRONT, GL_AMBIENT, m_Ambient.GetRepr().data());
  f->glMaterialfv(GL_FRONT, GL_DIFFUSE, m_Diffuse.GetRepr().data());
  f->glMaterialfv(GL_FRONT, GL_SPECULAR, m_Specular.GetRepr().data());
  f->glMaterialf(GL_FRONT, GL_SHININESS, m_Shine);
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
