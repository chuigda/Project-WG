#ifndef PROJECT_WG_COLOR_H
#define PROJECT_WG_COLOR_H

#include <array>
#include <cstdint>

#include <QtGui/qopengl.h>

class QOpenGLFunctions_2_0;

namespace cw {

class RGBAColor {
public:
  explicit constexpr RGBAColor(std::array<std::uint8_t, 4> repr) noexcept
    : m_Repr(repr)
  {}

  explicit constexpr RGBAColor(std::uint8_t red,
                               std::uint8_t green,
                               std::uint8_t blue,
                               std::uint8_t alpha = 255) noexcept
    : m_Repr({ red, green, blue, alpha })
  {}

  [[nodiscard]]
  constexpr inline std::uint8_t GetRed() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]]
  constexpr inline std::uint8_t GetGreen() const noexcept {
    return m_Repr[1];
  }

  [[nodiscard]]
  constexpr inline std::uint8_t GetBlue() const noexcept {
    return m_Repr[2];
  }

  [[nodiscard]]
  constexpr inline std::uint8_t GetAlpha() const noexcept {
    return m_Repr[3];
  }

  [[nodiscard]]
  constexpr inline std::array<std::uint8_t, 4> GetRepr() const noexcept {
    return m_Repr;
  }

  [[nodiscard]]
  constexpr inline bool IsTransparent() const noexcept {
    return m_Repr[3] != 255;
  }

  void Apply(QOpenGLFunctions_2_0 *f) noexcept;

private:
  std::array<std::uint8_t, 4> m_Repr;
};

static_assert(sizeof(RGBAColor) == sizeof(std::array<std::uint8_t, 4>),
              "RGBAColor must be the same size as std::array<std::uint8_t, 4>");

constexpr RGBAColor g_White { static_cast<std::uint8_t>(255), 255, 255, 255 };

class RGBAColorFloat {
public:
  explicit RGBAColorFloat(const std::array<float, 4> &repr) noexcept;
  explicit RGBAColorFloat(GLfloat red,
                          GLfloat green,
                          GLfloat blue,
                          GLfloat alpha = 1.0f) noexcept;
  explicit RGBAColorFloat(RGBAColor color) noexcept;

  [[nodiscard]]
  constexpr inline GLfloat GetRed() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]]
  constexpr inline GLfloat GetGreen() const noexcept {
    return m_Repr[1];
  }

  [[nodiscard]]
  constexpr inline GLfloat GetBlue() const noexcept {
    return m_Repr[2];
  }

  [[nodiscard]]
  constexpr inline GLfloat GetAlpha() const noexcept {
    return m_Repr[3];
  }

  [[nodiscard]]
  constexpr inline std::array<GLfloat, 4> const& GetRepr() const noexcept {
    return m_Repr;
  }

  [[nodiscard]]
  constexpr inline bool IsTransparent() const noexcept {
    return m_Repr[3] <= 0.99;
  }

  void Apply(QOpenGLFunctions_2_0 *f) const noexcept;

private:
  std::array<GLfloat, 4> m_Repr;
};

void ApplyColorArray(QOpenGLFunctions_2_0 *f,
                     const RGBAColorFloat* colors,
                     std::size_t numColor) noexcept;

static_assert(sizeof(RGBAColorFloat) == sizeof(std::array<GLfloat, 4>),
              "RGBAColorFloat must be the same size as std::array<GLfloat, 4>");

} // namespace cw

#endif // PROJECT_WG_COLOR_H
