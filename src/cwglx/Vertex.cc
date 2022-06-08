#include "cwglx/Vertex.h"

namespace cw {

Vertex2D::Vertex2D(GLdouble x, GLdouble y) noexcept
  : m_Repr({x, y})
{}

Vertex::Vertex(GLdouble x, GLdouble y, GLdouble z) noexcept
  : m_Repr({x, y, z})
{}

Vertex &Vertex::operator+=(const Vector &rhs) noexcept {
  m_Repr[0] += rhs.GetX();
  m_Repr[1] += rhs.GetY();
  m_Repr[2] += rhs.GetZ();
  return *this;
}

Vertex &Vertex::operator-=(const Vector &rhs) noexcept {
  m_Repr[0] -= rhs.GetX();
  m_Repr[1] -= rhs.GetY();
  m_Repr[2] -= rhs.GetZ();
  return *this;
}

Vector::Vector(GLdouble x, GLdouble y, GLdouble z) noexcept
  : m_Repr({x, y, z})
{}

Vector &Vector::operator+=(const Vector &rhs) noexcept {
  m_Repr[0] += rhs.GetX();
  m_Repr[1] += rhs.GetY();
  m_Repr[2] += rhs.GetZ();
  return *this;
}

Vector &Vector::operator-=(const Vector &rhs) noexcept {
  m_Repr[0] -= rhs.GetX();
  m_Repr[1] -= rhs.GetY();
  m_Repr[2] -= rhs.GetZ();
  return *this;
}

Vector &Vector::operator*=(GLdouble rhs) noexcept {
  m_Repr[0] *= rhs;
  m_Repr[1] *= rhs;
  m_Repr[2] *= rhs;
  return *this;
}

Vector operator+(const Vector& lhs, const Vector& rhs) noexcept {
  return Vector(lhs.GetX() + rhs.GetX(),
                lhs.GetY() + rhs.GetY(),
                lhs.GetZ() + rhs.GetZ());
}

Vector operator-(const Vector& lhs, const Vector& rhs) noexcept {
  return Vector(lhs.GetX() - rhs.GetX(),
                lhs.GetY() - rhs.GetY(),
                lhs.GetZ() - rhs.GetZ());
}

Vector operator*(const Vector& lhs, GLdouble rhs) noexcept {
  Vector ret = lhs;
  ret *= rhs;
  return ret;
}

Vector operator*(GLdouble lhs, const Vector& rhs) noexcept {
  return rhs * lhs;
}

Vector operator*(const Vector& lhs, const Vector& rhs) noexcept {
  const auto& [l, m, n] = lhs.GetRepr();
  const auto& [o, p, q] = rhs.GetRepr();

  return Vector(m * q - n * p,
                n * o - l * q,
                l * p - m * o);
}

Vertex operator+(const Vertex& lhs, const Vector& rhs) noexcept {
  Vertex ret = lhs;
  ret += rhs;
  return ret;
}

Vertex operator-(const Vertex& lhs, const Vector& rhs) noexcept {
  Vertex ret = lhs;
  ret -= rhs;
  return ret;
}

Vertex operator+(const Vector& lhs, const Vertex& rhs) noexcept {
  return rhs + lhs;
}

Vertex operator-(const Vector& lhs, const Vertex& rhs) noexcept {
  return rhs - lhs;
}

} // namespace cw
