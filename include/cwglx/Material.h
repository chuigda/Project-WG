#ifndef PROJECT_WG_MATERIAL_H
#define PROJECT_WG_MATERIAL_H

#include <QtGui/qopengl.h>
#include "cwglx/Color.h"

class QOpenGLFunctions_2_0;

namespace cw {

class Material {
public:
  virtual ~Material() = 0;

  [[nodiscard]] virtual bool IsTransparent() const noexcept = 0;
  virtual void Apply(QOpenGLFunctions_2_0 *f) const noexcept = 0;
};

class StandardMaterial final : public Material {
public:
  StandardMaterial(RGBAColor ambient,
                   RGBAColor diffuse,
                   RGBAColor specular,
                   float shininess,
                   GLenum colorDimension = GL_AMBIENT_AND_DIFFUSE) noexcept;

  StandardMaterial(const RGBAColorF& ambient,
                   const RGBAColorF& diffuse,
                   const RGBAColorF& specular,
                   float shininess,
                   GLenum colorDimension = GL_AMBIENT_AND_DIFFUSE) noexcept;

  ~StandardMaterial() final;

  [[nodiscard]] bool IsTransparent() const noexcept final;
  void Apply(QOpenGLFunctions_2_0 *f) const noexcept final;

private:
  RGBAColorF m_Ambient;
  RGBAColorF m_Diffuse;
  RGBAColorF m_Specular;
  GLfloat m_Shine;
  GLenum m_ColorDimension;
};

#define CW_DEFINE_MATERIAL(NAME, \
                           AR, AG, AB, AA, \
                           DR, DG, DB, DA, \
                           SR, SG, SB, SA, \
                           SHINE) \
  const StandardMaterial* Get##NAME##Material() noexcept;
#include "cwglx/DefaultMaterials.inc"

} // namespace cw

#endif // PROJECT_WG_MATERIAL_H
