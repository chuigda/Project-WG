#include "cwglx/drawable/TriangleGenerator.h"

#include "util/Constants.h"

namespace cw {

TriangleGenerator::~TriangleGenerator() = default;

SimpleTriangle::SimpleTriangle(const std::array<Vertex, 3>& vertices)
  : m_Vertices(vertices),
    m_Generated(false)
{}

SimpleTriangle::~SimpleTriangle() = default;

bool SimpleTriangle::HasNextTriangle() {
  return m_Generated;
}

std::array<Vertex, 3> SimpleTriangle::NextTriangle() {
  m_Generated = false;
  return m_Vertices;
}

SimpleTriangle SimpleTriangle::Clone() noexcept {
  return SimpleTriangle { m_Vertices };
}

Positioner::Positioner(std::unique_ptr<TriangleGenerator> &&generator,
                       const Vector& position)
  : m_Generator(std::move(generator)),
    m_Position(position)
{}

Positioner::~Positioner() {}

bool Positioner::HasNextTriangle() {
  return m_Generator->HasNextTriangle();
}

std::array<Vertex, 3> Positioner::NextTriangle() {
  auto triangle = m_Generator->NextTriangle();
  for (auto& vertex : triangle) {
    vertex += m_Position;
  }
  return triangle;
}

Composer::Composer(std::vector<std::unique_ptr<TriangleGenerator>> &&generators)
  : m_Generators(std::move(generators))
{}

Composer::~Composer() = default;

bool Composer::HasNextTriangle() {
  while (!m_CurrentGenerator->get()->HasNextTriangle()
         && m_CurrentGenerator == m_Generators.cend()) {
    ++m_CurrentGenerator;
  }

  return m_CurrentGenerator != m_Generators.cend();
}

std::array<Vertex, 3> Composer::NextTriangle() {
  while (!m_CurrentGenerator->get()->HasNextTriangle()) {
    ++m_CurrentGenerator;
  }

  return m_CurrentGenerator->get()->NextTriangle();
}

FanGenerator::FanGenerator(const Vector &centerPoint,
                           GLdouble radius,
                           GLdouble startAngle,
                           GLdouble endAngle,
                           std::size_t count)
  : m_CenterPoint(centerPoint),
    m_Radius(radius),
    m_StartAngle(DegToRad(startAngle)),
    m_Count(count),
    m_PieceDegree(DegToRad(endAngle - startAngle) / static_cast<GLdouble>(count)),
    m_CurrentCount(0)
{}

FanGenerator::~FanGenerator() = default;

bool FanGenerator::HasNextTriangle() {
  return m_CurrentCount < m_Count;
}

std::array<Vertex, 3> FanGenerator::NextTriangle() {
  Q_ASSERT(HasNextTriangle());

  GLdouble pieceStartAngle =
      static_cast<GLdouble>(m_CurrentCount) * m_PieceDegree
      + m_StartAngle;
  GLdouble pieceEndAngle =
      static_cast<GLdouble>(m_CurrentCount + 1) * m_PieceDegree
      + m_StartAngle;

  GLdouble x1 = m_Radius * std::cos(pieceStartAngle);
  GLdouble y1 = m_Radius * std::sin(pieceStartAngle);

  GLdouble x2 = m_Radius * std::cos(pieceEndAngle);
  GLdouble y2 = m_Radius * std::sin(pieceEndAngle);

  m_CurrentCount += 1;

  return {
    m_CenterPoint.AsVertex(),
    m_CenterPoint + Vertex(x1, y1, 0.0),
    m_CenterPoint + Vertex(x2, y2, 0.0)
  };
}

} // namespace cw
