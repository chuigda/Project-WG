#ifndef PROJECT_WG_TRIANGLE_GENERATOR_H
#define PROJECT_WG_TRIANGLE_GENERATOR_H

#include <array>
#include <memory>
#include <vector>
#include "cwglx/Vertex.h"
#include "util/Derive.h"

namespace cw {

class TriangleGenerator {
public:
  virtual ~TriangleGenerator() = 0;

  virtual bool HasNextTriangle() = 0;
  virtual std::array<Vertex, 3> NextTriangle() = 0;
};

class SimpleTriangle final : public TriangleGenerator {
public:
  explicit SimpleTriangle(const std::array<Vertex, 3>& vertices);

  ~SimpleTriangle() final;

  CW_DERIVE_UNCOPYABLE(SimpleTriangle)

  bool HasNextTriangle() final;

  std::array<Vertex, 3> NextTriangle() final;

  SimpleTriangle Clone() noexcept;

private:
  std::array<Vertex, 3> m_Vertices;
  bool m_Generated;
};

class Positioner final : public TriangleGenerator {
public:
  Positioner(std::unique_ptr<TriangleGenerator> &&generator,
             const Vector& position);

  ~Positioner() final;

  CW_DERIVE_UNCOPYABLE(Positioner)

  bool HasNextTriangle() final;

  std::array<Vertex, 3> NextTriangle() final;

private:
  std::unique_ptr<TriangleGenerator> m_Generator;
  Vector m_Position;
};

class Composer final : public TriangleGenerator {
public:
  Composer(std::vector<std::unique_ptr<TriangleGenerator>> &&generators);

  ~Composer() final;

  CW_DERIVE_UNCOPYABLE(Composer)

  bool HasNextTriangle() final;

  std::array<Vertex, 3> NextTriangle() final;

private:
  std::vector<std::unique_ptr<TriangleGenerator>> m_Generators;
  std::vector<std::unique_ptr<TriangleGenerator>>::iterator m_CurrentGenerator;
};

class FanGenerator final : public TriangleGenerator {
public:
  FanGenerator(const Vector& centerPoint,
               GLdouble radius,
               GLdouble startAngle,
               GLdouble endAngle,
               std::size_t count);

  ~FanGenerator() final;

  CW_DERIVE_UNCOPYABLE(FanGenerator)

  bool HasNextTriangle() final;

  std::array<Vertex, 3> NextTriangle() final;

private:
  Vector m_CenterPoint;
  GLdouble m_Radius;
  GLdouble m_StartAngle;
  std::size_t m_Count;

  GLdouble m_PieceDegree;
  std::size_t m_CurrentCount;
};

} // namespace cw

#endif // PROJECT_WG_TRIANGLE_GENERATOR_H
