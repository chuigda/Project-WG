#include "cwglx/Vertex.h"

#include <QOpenGLFunctions_2_0>

namespace cw {

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

Vector Vector::Normalize() const noexcept {
  const double length = std::sqrt(GetX() * GetX()
                                  + GetY() * GetY()
                                  + GetZ() * GetZ());
  return Vector(GetX() / length, GetY() / length, GetZ() / length);
}

Vector Vector::ABS() const noexcept {
  return Vector(std::abs(GetX()), std::abs(GetY()), std::abs(GetZ()));
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

Vector operator-(const Vertex& lhs, const Vertex& rhs) noexcept {
  return Vector(lhs.GetX() - rhs.GetX(),
                lhs.GetY() - rhs.GetY(),
                lhs.GetZ() - rhs.GetZ());
}

Vertex2DF::Vertex2DF(GLfloat x, GLfloat y) noexcept
  : m_Repr({x, y})
{}

Vertex2DF Vertex2DF::Downscale(const Vertex2D &v) noexcept {
  return Vertex2DF { static_cast<GLfloat>(v.GetX()),
                     static_cast<GLfloat>(v.GetY()) };
}

Vertex RotateVertex(const Vertex &vertex,
                    const Vertex &centerPoint,
                    Axis axis,
                    GLdouble degree)
{
  Vector diff = vertex - centerPoint;
  const auto [x, y, z] = diff.GetRepr();
  double cos = std::cos(degree);
  double sin = std::sin(degree);
  switch (axis) {
    case Axis::XAxis:
      return Vertex(x,
                    y * cos - z * sin,
                    y * sin + z * cos);
    case Axis::YAxis:
      return Vertex(x * cos + z * sin,
                    y,
                    -x * sin + z * cos);
    case Axis::ZAxis:
      return Vertex(x * cos - y * sin,
                    x * sin + y * cos,
                    z);
  }

  Q_UNREACHABLE();
}

} // namespace cw

