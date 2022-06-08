#include "cwglx/Material.h"

#include <QOpenGLFunctions_2_0>

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

StandardMaterial::~StandardMaterial() = default;

bool StandardMaterial::IsTransparent() const noexcept {
  return m_Ambient.IsTransparent()
         || m_Diffuse.IsTransparent()
         || m_Specular.IsTransparent();
}

void StandardMaterial::Apply(QOpenGLFunctions_2_0 *f) const noexcept {
  f->glColorMaterial(GL_FRONT, m_ColorDimension);
  f->glMaterialfv(m_ColorDimension, GL_AMBIENT, m_Ambient.GetRepr().data());
  f->glMaterialfv(m_ColorDimension, GL_DIFFUSE, m_Diffuse.GetRepr().data());
  f->glMaterialfv(m_ColorDimension, GL_SPECULAR, m_Specular.GetRepr().data());
  f->glMaterialf(m_ColorDimension, GL_SHININESS, m_Shine);
}

#define CW_DEFINE_MATERIAL(NAME, \
                           AR, AG, AB, AA, \
                           DR, DG, DB, DA, \
                           SR, SG, SB, SA, \
                           SHINE) \
  const StandardMaterial* Get##NAME##Material() noexcept { \
    static const StandardMaterial material { \
      cw::RGBAColor(AR, AG, AB, AA), \
      cw::RGBAColor(DR, DG, DB, DA), \
      cw::RGBAColor(SR, SG, SB, SA), \
      SHINE \
    }; \
    return &material; \
  }
#include "cwglx/DefaultMaterials.inc"

} // namespace cw
