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

static_assert(sizeof(Vertex) == sizeof(std::array<GLdouble, 3>),
              "Vertex must be the same size as std::array<GLdouble, 3>");

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

static_assert(sizeof(Vector) == sizeof(std::array<GLdouble, 3>),
              "Vector must be the same size as std::array<GLdouble, 3>");

Vector operator+(const Vector& lhs, const Vector& rhs) noexcept;

Vector operator-(const Vector& lhs, const Vector& rhs) noexcept;

Vector operator*(const Vector& lhs, GLdouble rhs) noexcept;

Vector operator*(GLdouble lhs, const Vector& rhs) noexcept;

Vector operator*(const Vector& lhs, const Vector& rhs) noexcept;

Vertex operator+(const Vertex& lhs, const Vector& rhs) noexcept;

Vertex operator-(const Vertex& lhs, const Vector& rhs) noexcept;

Vertex operator+(const Vector& lhs, const Vertex& rhs) noexcept;

Vertex operator-(const Vector& lhs, const Vertex& rhs) noexcept;

class Vertex2DF final {
public:
  explicit Vertex2DF(GLfloat x, GLfloat y) noexcept;

  [[nodiscard]]
  static Vertex2DF Downscale(const Vertex2D& v) noexcept;

  [[nodiscard]]
  constexpr inline GLfloat GetX() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]]
  constexpr inline GLfloat GetY() const noexcept {
    return m_Repr[1];
  }

  [[nodiscard]]
  constexpr inline const std::array<GLfloat, 2>& GetRepr() const noexcept {
    return m_Repr;
  }

private:
  std::array<GLfloat, 2> m_Repr;
};

static_assert(sizeof(Vertex2DF) == sizeof(std::array<GLfloat, 2>),
              "Vertex2DF must be the same size as std::array<GLfloat, 2>");

class VertexF final {
public:
  explicit VertexF(GLfloat x, GLfloat y, GLfloat z = 0.0f) noexcept;

  [[nodiscard]]
  static VertexF Downscale(const Vertex& v) noexcept;

  [[nodiscard]]
  constexpr inline GLfloat GetX() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]]
  constexpr inline GLfloat GetY() const noexcept {
    return m_Repr[1];
  }

  [[nodiscard]]
  constexpr inline GLfloat GetZ() const noexcept {
    return m_Repr[2];
  }

  [[nodiscard]]
  constexpr inline const std::array<GLfloat, 3>& GetRepr() const noexcept {
    return m_Repr;
  }

private:
  std::array<GLfloat, 3> m_Repr;
};

static_assert(sizeof(VertexF) == sizeof(std::array<GLfloat, 3>),
              "VertexF must be the same size as std::array<GLfloat, 3>");

class VectorF final {
public:
  explicit VectorF(GLfloat x, GLfloat y, GLfloat z = 0.0f) noexcept;

  [[nodiscard]]
  static VectorF Downscale(const Vector& v) noexcept;

  [[nodiscard]]
  constexpr inline GLfloat GetX() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]]
  constexpr inline GLfloat GetY() const noexcept {
    return m_Repr[1];
  }

  [[nodiscard]]
  constexpr inline GLfloat GetZ() const noexcept {
    return m_Repr[2];
  }

  [[nodiscard]]
  constexpr inline const std::array<GLfloat, 3>& GetRepr() const noexcept {
    return m_Repr;
  }

private:
  std::array<GLfloat, 3> m_Repr;
};

static_assert(sizeof(VectorF) == sizeof(std::array<GLfloat, 3>),
              "VectorF must be the same size as std::array<GLfloat, 3>");

} // namespace cw

#endif //PROJECT_WG_VERTEX_H