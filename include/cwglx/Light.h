#ifndef PROJECT_WG_LIGHT_H
#define PROJECT_WG_LIGHT_H

#include <array>

#include "cwglx/GL.h"
#include "cwglx/Color.h"
#include "cwglx/Vertex.h"

namespace cw {

class Light {
public:
  explicit Light(GLenum lightId,
                 RGBAColor ambient,
                 RGBAColor diffuse,
                 RGBAColor specular,
                 GLFunctions *f) noexcept;

  virtual ~Light() = 0;

  virtual void Enable(GLFunctions *f) const noexcept = 0;
  void Disable(GLFunctions *f) const noexcept;

  [[nodiscard]]
  constexpr inline GLenum GetLightId() const noexcept {
    return m_LightId;
  }

  [[nodiscard]]
  constexpr inline const RGBAColorF& GetAmbient() const noexcept {
    return m_Ambient;
  }

  [[nodiscard]]
  constexpr inline const RGBAColorF& GetDiffuse() const noexcept {
    return m_Diffuse;
  }

  [[nodiscard]]
  constexpr inline const RGBAColorF& GetSpecular() const noexcept {
    return m_Specular;
  }

private:
  GLenum m_LightId;
  RGBAColorF m_Ambient;
  RGBAColorF m_Diffuse;
  RGBAColorF m_Specular;
};

class ParallelLight final : public Light {
public:
  explicit ParallelLight(GLenum lightId,
                         RGBAColor ambient,
                         RGBAColor diffuse,
                         RGBAColor specular,
                         const Vector &direction,
                         GLFunctions *f) noexcept;

  ~ParallelLight() override = default;

  void Enable(GLFunctions *f) const noexcept override;

private:
  std::array<GLfloat, 4> m_DirectionRepr;
};

class PointLight final : public Light {
public:
  explicit PointLight(GLenum lightId,
                      RGBAColor ambient,
                      RGBAColor diffuse,
                      RGBAColor specular,
                      const Vertex &position,
                      GLFunctions *f) noexcept;

  ~PointLight() override = default;

  void Enable(GLFunctions *f) const noexcept override;

private:
  std::array<GLfloat, 4> m_PositionRepr;
};

} // namespace cw

#endif //PROJECT_WG_LIGHT_H
