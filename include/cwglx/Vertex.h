#ifndef PROJECT_WG_VERTEX_H
#define PROJECT_WG_VERTEX_H

#include <array>
#include <QtGui/qopengl.h>

namespace cw {

class Vector;

class Vertex2D final {
public:
  explicit Vertex2D(GLdouble x, GLdouble y) noexcept;

  [[nodiscard]]
  constexpr inline GLdouble GetX() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]]
  constexpr inline GLdouble GetY() const noexcept {
    return m_Repr[1];
  }

  [[nodiscard]]
  constexpr inline const std::array<GLdouble, 2> &GetRepr() const noexcept {
    return m_Repr;
  }

private:
  std::array<GLdouble, 2> m_Repr;
};

static_assert(sizeof(Vertex2D) == sizeof(GLdouble) * 2,
              "Vertex2D must be exactly 2 GLdouble's in size");

class Vertex final {
public:
  explicit Vertex(GLdouble x, GLdouble y, GLdouble z = 0.0) noexcept;

  [[nodiscard]]
  constexpr inline GLdouble GetX() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]]
  constexpr inline GLdouble GetY() const noexcept {
    return m_Repr[1];
  }

  [[nodiscard]]
  constexpr inline GLdouble GetZ() const noexcept {
    return m_Repr[2];
  }

  [[nodiscard]]
  constexpr inline const std::array<GLdouble, 3>& GetRepr() const noexcept {
    return m_Repr;
  }

  Vertex& operator+=(const Vector& rhs) noexcept;

  Vertex& operator-=(const Vector& rhs) noexcept;

private:
  std::array<GLdouble, 3> m_Repr;
};

static_assert(sizeof(Vertex) == sizeof(GLdouble) * 3,
              "Vertex must be exactly 3 GLdouble's in size");

class Vector final {
public:
  explicit Vector(GLdouble x, GLdouble y, GLdouble z = 0.0) noexcept;

  [[nodiscard]]
  constexpr inline GLdouble GetX() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]]
  constexpr inline GLdouble GetY() const noexcept {
    return m_Repr[1];
  }

  [[nodiscard]]
  constexpr inline GLdouble GetZ() const noexcept {
    return m_Repr[2];
  }

  [[nodiscard]]
  constexpr inline const std::array<GLdouble, 3>& GetRepr() const noexcept {
    return m_Repr;
  }

  Vector& operator+=(const Vector& rhs) noexcept;

  Vector& operator-=(const Vector& rhs) noexcept;

  Vector& operator*=(GLdouble rhs) noexcept;

private:
  std::array<GLdouble, 3> m_Repr;
};

static_assert(sizeof(Vector) == sizeof(GLdouble) * 3,
              "Vector must be exactly 3 GLdouble's in size");

Vector operator+(const Vector& lhs, const Vector& rhs) noexcept;

Vector operator-(const Vector& lhs, const Vector& rhs) noexcept;

Vector operator*(const Vector& lhs, GLdouble rhs) noexcept;

Vector operator*(GLdouble lhs, const Vector& rhs) noexcept;

Vector operator*(const Vector& lhs, const Vector& rhs) noexcept;

Vertex operator+(const Vertex& lhs, const Vector& rhs) noexcept;

Vertex operator-(const Vertex& lhs, const Vector& rhs) noexcept;

Vertex operator+(const Vector& lhs, const Vertex& rhs) noexcept;

Vertex operator-(const Vector& lhs, const Vertex& rhs) noexcept;

} // namespace cw

#endif //PROJECT_WG_VERTEX_H
