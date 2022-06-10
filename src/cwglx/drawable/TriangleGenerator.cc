#include "cwglx/drawable/TriangleGenerator.h"

namespace cw {

TriangleGenerator::~TriangleGenerator() = default;

SimpleTriangle::~SimpleTriangle() = default;

bool SimpleTriangle::HasNextTriangle() {
  return m_Generated;
}

std::array<Vertex, 3> SimpleTriangle::NextTriangle() {
  m_Generated = false;
  return m_Vertices;
}

} // namespace cw
