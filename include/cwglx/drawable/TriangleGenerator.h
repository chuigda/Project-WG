#ifndef PROJECT_WG_TRIANGLE_GENERATOR_H
#define PROJECT_WG_TRIANGLE_GENERATOR_H

#include <array>
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
  ~SimpleTriangle() final;

  CW_DERIVE_UNCOPYABLE(SimpleTriangle)

  bool HasNextTriangle() final;

  std::array<Vertex, 3> NextTriangle() final;

private:
  std::array<Vertex, 3> m_Vertices;
  bool m_Generated;
};

} // namespace cw

#endif // PROJECT_WG_TRIANGLE_GENERATOR_H
