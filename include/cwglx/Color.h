#ifndef PROJECT_WG_COLOR_H
#define PROJECT_WG_COLOR_H

#include <array>
#include <cstdint>
#include <QtGui/qopengl.h>

#include "cwglx/GL.h"

namespace cw {

class RGBAColor {
public:
  explicit constexpr inline RGBAColor(std::array<std::uint8_t, 4> repr) noexcept
    : m_Repr(repr)
  {}

  explicit constexpr inline RGBAColor(std::uint8_t red,
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

  constexpr inline void SetAlpha(std::uint8_t alpha) noexcept {
    m_Repr[3] = alpha;
  }

  [[nodiscard]]
  constexpr inline std::array<std::uint8_t, 4> GetRepr() const noexcept {
    return m_Repr;
  }

  [[nodiscard]]
  constexpr std::array<std::uint8_t, 4>& GetRawRepr() noexcept {
    return m_Repr;
  }

  [[nodiscard]]
  constexpr inline bool IsTransparent() const noexcept {
    return m_Repr[3] != 255;
  }

  void Apply(GLFunctions *f) noexcept;

private:
  std::array<std::uint8_t, 4> m_Repr;
};

static_assert(sizeof(RGBAColor) == sizeof(std::array<std::uint8_t, 4>),
              "RGBAColor must be the same size as std::array<std::uint8_t, 4>");

constexpr RGBAColor g_White { static_cast<std::uint8_t>(255), 255, 255, 255 };

class RGBAColorF {
public:
  explicit constexpr inline
  RGBAColorF(const std::array<float, 4> &repr) noexcept
    : m_Repr(repr)
  {}

  constexpr inline RGBAColorF(GLfloat red,
                              GLfloat green,
                              GLfloat blue,
                              GLfloat alpha) noexcept
    : m_Repr({red, green, blue, alpha})
  {}

  explicit constexpr inline RGBAColorF(RGBAColor color) noexcept
    : m_Repr({ static_cast<GLfloat>(color.GetRed()) / 255.0f,
               static_cast<GLfloat>(color.GetGreen()) / 255.0f,
               static_cast<GLfloat>(color.GetBlue()) / 255.0f,
               static_cast<GLfloat>(color.GetAlpha()) / 255.0f })
  {}

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

  void Apply(GLFunctions *f) const noexcept;

private:
  std::array<GLfloat, 4> m_Repr;
};

void ApplyColorArray(GLFunctions *f,
                     const RGBAColorF* colors,
                     std::size_t numColor) noexcept;

static_assert(sizeof(RGBAColorF) == sizeof(std::array<GLfloat, 4>),
              "RGBAColorF must be the same size as std::array<GLfloat, 4>");

} // namespace cw

#endif // PROJECT_WG_COLOR_H
