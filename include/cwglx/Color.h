#ifndef PROJECT_WG_COLOR_H
#define PROJECT_WG_COLOR_H

#include <array>
#include <cstdint>

#include <QtGui/qopengl.h>

class QOpenGLFunctions_2_0;

namespace cw {

class RGBAColor {
public:
  explicit RGBAColor(std::array<std::uint8_t, 4> repr) noexcept;
  explicit RGBAColor(std::uint8_t red,
                     std::uint8_t green,
                     std::uint8_t blue,
                     std::uint8_t alpha = 255) noexcept;
  explicit RGBAColor(float red,
                     float green,
                     float blue,
                     float alpha = 1.0f) noexcept;

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

  void Apply(QOpenGLFunctions_2_0 *f) noexcept;

private:
  std::array<std::uint8_t, 4> m_Repr;
};

class RGBAColorFloat {
public:
  explicit RGBAColorFloat(std::array<float, 4> repr) noexcept;
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

  void Apply(QOpenGLFunctions_2_0 *f) noexcept;

private:
  std::array<GLfloat, 4> m_Repr;
};

} // namespace cw

#endif // PROJECT_WG_COLOR_H
