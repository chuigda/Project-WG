#include "cwglx/Light.h"

#include "cwglx/GLImpl.h"

namespace cw {

Light::Light(GLenum lightId,
             RGBAColor ambient,
             RGBAColor diffuse,
             RGBAColor specular,
             GLFunctions *f) noexcept
  : m_LightId(lightId),
    m_Ambient(ambient),
    m_Diffuse(diffuse),
    m_Specular(specular)
{
  f->glLightfv(m_LightId, GL_AMBIENT, m_Ambient.GetRepr().data());
  f->glLightfv(m_LightId, GL_DIFFUSE, m_Diffuse.GetRepr().data());
  f->glLightfv(m_LightId, GL_SPECULAR, m_Specular.GetRepr().data());
}

void Light::Enable(GLFunctions *f) const noexcept {
  f->glEnable(m_LightId);
}

void Light::Disable(GLFunctions *f) const noexcept {
  f->glDisable(m_LightId);
}

Light::~Light() = default;

ParallelLight::ParallelLight(GLenum lightId,
                             RGBAColor ambient,
                             RGBAColor diffuse,
                             RGBAColor specular,
                             const Vector &direction,
                             GLFunctions *f) noexcept
  : Light(lightId, ambient, diffuse, specular, f),
    m_DirectionRepr {
        static_cast<GLfloat>(-direction.GetX()),
        static_cast<GLfloat>(-direction.GetY()),
        static_cast<GLfloat>(-direction.GetZ()),
        0.0f
    }
{}

void ParallelLight::Enable(GLFunctions *f) const noexcept {
  f->glPushMatrix();
  f->glLoadIdentity();
  f->glLightfv(GetLightId(), GL_POSITION, m_DirectionRepr.data());
  f->glEnable(GetLightId());
  f->glPopMatrix();
}

PointLight::PointLight(GLenum lightId,
                       RGBAColor ambient,
                       RGBAColor diffuse,
                       RGBAColor specular,
                       const Vertex &position,
                       GLFunctions *f) noexcept
  : Light(lightId, ambient, diffuse, specular, f),
    m_PositionRepr {
        static_cast<GLfloat>(position.GetX()),
        static_cast<GLfloat>(position.GetY()),
        static_cast<GLfloat>(position.GetZ()),
        1.0f
    }
{}

void PointLight::Enable(GLFunctions *f) const noexcept {
  f->glLightfv(GetLightId(), GL_POSITION, m_PositionRepr.data());
  f->glEnable(GetLightId());
}

} // namespace cw
