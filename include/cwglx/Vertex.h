#ifndef PROJECT_WG_VERTEX_H
#define PROJECT_WG_VERTEX_H

#include <array>
#include <cmath>
#include <QtGui/qopengl.h>

#include "cwglx/GL.h"
#include "cwglx/Defs.h"

namespace cw {

class Vector;

class Vertex final {
public:
  constexpr inline
  Vertex(GLdouble x, GLdouble y, GLdouble z = 0.0) noexcept
    : m_Repr({x, y, z})
  {}

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

  constexpr inline void SetX(GLdouble x) noexcept {
    m_Repr[0] = x;
  }

  constexpr inline void SetY(GLdouble y) noexcept {
    m_Repr[1] = y;
  }

  constexpr inline void SetZ(GLdouble z) noexcept {
    m_Repr[2] = z;
  }

  [[nodiscard]]
  constexpr inline const std::array<GLdouble, 3>& GetRepr() const noexcept {
    return m_Repr;
  }

  constexpr inline Vertex& operator+=(const Vector& rhs) noexcept;

  constexpr inline Vertex& operator-=(const Vector& rhs) noexcept;

private:
  std::array<GLdouble, 3> m_Repr;
};

static_assert(sizeof(Vertex) == sizeof(std::array<GLdouble, 3>),
              "Vertex must be the same size as std::array<GLdouble, 3>");

class Vector final {
public:
  constexpr inline
  Vector(GLdouble x, GLdouble y, GLdouble z = 0.0) noexcept
    : m_Repr({x, y, z})
  {}

  [[nodiscard]]
  constexpr inline static
  Vector FromVertex(const Vertex& vertex) noexcept {
    return {
        vertex.GetX(),
        vertex.GetY(),
        vertex.GetZ()
    };
  }

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

  constexpr inline Vector& operator+=(const Vector& rhs) noexcept;

  constexpr inline Vector& operator-=(const Vector& rhs) noexcept;

  constexpr inline Vector& operator*=(GLdouble rhs) noexcept;

  [[nodiscard]]
  constexpr inline Vector Normalize() const noexcept {
    const double length = std::sqrt(GetX() * GetX()
                                    + GetY() * GetY()
                                    + GetZ() * GetZ());
    return { GetX() / length, GetY() / length, GetZ() / length };
  }

  [[nodiscard]]
  constexpr inline Vector ABS() const noexcept {
    return {
        std::abs(GetX()),
        std::abs(GetY()),
        std::abs(GetZ())
    };
  }

  [[nodiscard]]
  constexpr inline Vertex AsVertex() const noexcept {
    return { GetX(), GetY(), GetZ() };
  }

  [[nodiscard]]
  constexpr inline GLdouble Length() const noexcept {
    return std::sqrt(GetX() * GetX() + GetY() * GetY() + GetZ() * GetZ());
  }

private:
  std::array<GLdouble, 3> m_Repr;
};

static_assert(sizeof(Vector) == sizeof(std::array<GLdouble, 3>),
              "Vector must be the same size as std::array<GLdouble, 3>");

constexpr inline Vertex& Vertex::operator+=(const Vector& rhs) noexcept {
  m_Repr[0] += rhs.GetX();
  m_Repr[1] += rhs.GetY();
  m_Repr[2] += rhs.GetZ();
  return *this;
}

constexpr inline Vertex& Vertex::operator-=(const Vector& rhs) noexcept {
  m_Repr[0] -= rhs.GetX();
  m_Repr[1] -= rhs.GetY();
  m_Repr[2] -= rhs.GetZ();
  return *this;
}

constexpr inline Vector& Vector::operator+=(const Vector& rhs) noexcept {
  m_Repr[0] += rhs.GetX();
  m_Repr[1] += rhs.GetY();
  m_Repr[2] += rhs.GetZ();
  return *this;
}

constexpr inline Vector& Vector::operator-=(const Vector& rhs) noexcept {
  m_Repr[0] -= rhs.GetX();
  m_Repr[1] -= rhs.GetY();
  m_Repr[2] -= rhs.GetZ();
  return *this;
}

constexpr inline Vector& Vector::operator*=(GLdouble rhs) noexcept {
  m_Repr[0] *= rhs;
  m_Repr[1] *= rhs;
  m_Repr[2] *= rhs;
  return *this;
}

[[nodiscard]]
constexpr inline
Vector operator+(const Vector& lhs, const Vector& rhs) noexcept {
  return {
      lhs.GetX() + rhs.GetX(),
      lhs.GetY() + rhs.GetY(),
      lhs.GetZ() + rhs.GetZ()
  };
}

[[nodiscard]]
constexpr inline
Vector operator-(const Vector& lhs, const Vector& rhs) noexcept {
  return {
      lhs.GetX() - rhs.GetX(),
      lhs.GetY() - rhs.GetY(),
      lhs.GetZ() - rhs.GetZ()
  };
}

[[nodiscard]]
constexpr inline
Vector operator*(const Vector& lhs, GLdouble rhs) noexcept {
  Vector ret = lhs;
  ret *= rhs;
  return ret;
}

[[nodiscard]]
constexpr inline
Vector operator*(GLdouble lhs, const Vector& rhs) noexcept {
  return rhs * lhs;
}

[[nodiscard]]
constexpr inline
Vector operator*(const Vector& lhs, const Vector& rhs) noexcept {
  const auto& [l, m, n] = lhs.GetRepr();
  const auto& [o, p, q] = rhs.GetRepr();

  return {
      m * q - n * p,
      n * o - l * q,
      l * p - m * o
  };
}

[[nodiscard]]
constexpr inline
Vertex operator+(const Vertex& lhs, const Vector& rhs) noexcept {
  Vertex ret = lhs;
  ret += rhs;
  return ret;
}

[[nodiscard]]
constexpr inline
Vertex operator-(const Vertex& lhs, const Vector& rhs) noexcept {
  Vertex ret = lhs;
  ret -= rhs;
  return ret;
}

[[nodiscard]]
constexpr inline
Vertex operator+(const Vector& lhs, const Vertex& rhs) noexcept {
  return rhs + lhs;
}

[[nodiscard]]
constexpr inline
Vertex operator-(const Vector& lhs, const Vertex& rhs) noexcept {
  return rhs - lhs;
}

[[nodiscard]]
constexpr inline
Vector operator-(const Vertex& lhs, const Vertex& rhs) noexcept {
  return {
      lhs.GetX() - rhs.GetX(),
      lhs.GetY() - rhs.GetY(),
      lhs.GetZ() - rhs.GetZ()
  };
}

class VertexF final {
public:
  constexpr inline
  VertexF(GLfloat x, GLfloat y, GLfloat z = 0.0f) noexcept
    : m_Repr({x, y, z})
  {}

  [[nodiscard]]
  constexpr inline static VertexF Downscale(const Vertex& v) noexcept {
    return VertexF {
        static_cast<GLfloat>(v.GetX()),
        static_cast<GLfloat>(v.GetY()),
        static_cast<GLfloat>(v.GetZ())
    };
  }

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

  constexpr inline void SetX(GLfloat x) noexcept {
    m_Repr[0] = x;
  }

  constexpr inline void SetY(GLfloat y) noexcept {
    m_Repr[1] = y;
  }

  constexpr inline void SetZ(GLfloat z) noexcept {
    m_Repr[2] = z;
  }

private:
  std::array<GLfloat, 3> m_Repr;
};

static_assert(sizeof(VertexF) == sizeof(std::array<GLfloat, 3>),
              "VertexF must be the same size as std::array<GLfloat, 3>");

class Vertex2DF final {
public:
  constexpr inline Vertex2DF() noexcept : Vertex2DF(0.0f, 0.0f) {}

  constexpr inline Vertex2DF(GLfloat x, GLfloat y) noexcept
    : m_Repr({ x, y })
  {}

  [[nodiscard]] constexpr inline GLfloat GetX() const noexcept {
    return m_Repr[0];
  }

  [[nodiscard]] constexpr inline GLfloat GetY() const noexcept {
    return m_Repr[1];
  }

  constexpr inline void SetX(GLfloat x) noexcept {
    m_Repr[0] = x;
  }

  constexpr inline void SetY(GLfloat y) noexcept {
    m_Repr[1] = y;
  }

private:
  std::array<GLfloat, 2> m_Repr;
};

class VectorF final {
public:
  constexpr inline
  VectorF(GLfloat x, GLfloat y, GLfloat z = 0.0f) noexcept
    : m_Repr({x, y, z})
  {}

  [[nodiscard]]
  constexpr inline static VectorF Downscale(const Vector& v) noexcept {
    return {
        static_cast<GLfloat>(v.GetX()),
        static_cast<GLfloat>(v.GetY()),
        static_cast<GLfloat>(v.GetZ())
    };
  }

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

Vertex RotateVertex(const Vertex &vertex,
                    const Vertex &centerPoint,
                    Axis axis,
                    GLdouble degree);

namespace constants {

constexpr Vertex g_ZeroVertex { 0.0, 0.0, 0.0 };
constexpr Vector g_ZeroVector { 0.0, 0.0, 0.0 };
constexpr Vector g_UnitVectorX { 1.0, 0.0, 0.0 };
constexpr Vector g_UnitVectorY { 0.0, 1.0, 0.0 };
constexpr Vector g_UnitVectorZ { 0.0, 0.0, 1.0 };

} // namespace constants

using Triangle = std::array<Vertex, 3>;
using TriangleNormal = std::array<Vector, 3>;

using TriangleF = std::array<VertexF, 3>;
using TriangleNormalF = std::array<VectorF, 3>;

using Rect = std::array<Vertex, 4>;
using RectNormal = std::array<Vector, 4>;

using RectF = std::array<VertexF, 4>;
using RectNormalF = std::array<VectorF, 4>;

[[nodiscard]]
constexpr inline TriangleF
DownscaleTriangle(const Triangle &triangle) noexcept {
  return {
      VertexF::Downscale(triangle[0]),
      VertexF::Downscale(triangle[1]),
      VertexF::Downscale(triangle[2])
  };
}

[[nodiscard]]
constexpr inline TriangleNormalF
DownscaleTriangleNormal(const TriangleNormal &triangle) noexcept {
  return {
      VectorF::Downscale(triangle[0]),
      VectorF::Downscale(triangle[1]),
      VectorF::Downscale(triangle[2])
  };
}

[[nodiscard]]
constexpr inline RectF DownscaleRect(const Rect &rect) noexcept {
  return {
      VertexF::Downscale(rect[0]),
      VertexF::Downscale(rect[1]),
      VertexF::Downscale(rect[2]),
      VertexF::Downscale(rect[3])
  };
}

[[nodiscard]]
constexpr inline RectNormalF
DownscaleRectNormal(const RectNormal &rect) noexcept {
  return {
      VectorF::Downscale(rect[0]),
      VectorF::Downscale(rect[1]),
      VectorF::Downscale(rect[2]),
      VectorF::Downscale(rect[3])
  };
}

} // namespace cw

#endif //PROJECT_WG_VERTEX_H
