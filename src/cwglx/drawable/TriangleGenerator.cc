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
  : m_Generators(std::move(generators)),
    m_CurrentGenerator(m_Generators.begin())
{}

Composer::~Composer() = default;

bool Composer::HasNextTriangle() {
  while (m_CurrentGenerator != m_Generators.cend()
         && !m_CurrentGenerator->get()->HasNextTriangle()) {
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
                           std::size_t count,
                           CircleAxis axis)
  : m_CenterPoint(centerPoint),
    m_Radius(radius),
    m_StartAngle(DegToRad(startAngle)),
    m_Count(count),
    m_Axis(axis),
    m_PieceDegree(DegToRad(endAngle - startAngle)
                  / static_cast<GLdouble>(count)),
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

  switch (m_Axis) {
  case CircleAxis::XAxis:
    return {
        m_CenterPoint.AsVertex(),
        m_CenterPoint + Vertex(0.0, y2, x2),
        m_CenterPoint + Vertex(0.0, y1, x1),
    };
  case CircleAxis::YAxis:
    return {
        m_CenterPoint.AsVertex(),
        m_CenterPoint + Vertex(x2, 0.0, y2),
        m_CenterPoint + Vertex(x1, 0.0, y1)
    };
  case CircleAxis::ZAxis:
    return {
        m_CenterPoint.AsVertex(),
        m_CenterPoint + Vertex(x1, y1, 0.0),
        m_CenterPoint + Vertex(x2, y2, 0.0)
    };
  }
}

CylinderGenerator::CylinderGenerator(const Vector &centerPoint,
                                     GLdouble radius,
                                     GLdouble height,
                                     GLdouble startAngle,
                                     GLdouble endAngle,
                                     std::size_t count,
                                     CircleAxis axis)
  : m_CenterPoint(centerPoint),
    m_Radius(radius),
    m_HalfHeight(height / 2.0),
    m_StartAngle(axis == CircleAxis::YAxis ? DegToRad(startAngle)
                                           : -DegToRad(startAngle)),
    m_Count(count),
    m_Axis(axis),

    m_PieceDegree((axis == CircleAxis::YAxis ? DegToRad(endAngle - startAngle)
                                             : DegToRad(startAngle - endAngle))
                  / static_cast<GLdouble>(count)),
    m_CurrentCount(0),
    m_UpTriangle(true)
{}

CylinderGenerator::~CylinderGenerator() = default;

bool CylinderGenerator::HasNextTriangle() {
  return m_CurrentCount < m_Count;
}

std::array<Vertex, 3> CylinderGenerator::NextTriangle() {
  Q_ASSERT(HasNextTriangle());

  GLdouble pieceStartAngle =
      static_cast<GLdouble>(m_CurrentCount) * m_PieceDegree
      + m_StartAngle;
  GLdouble pieceEndAngle =
      static_cast<GLdouble>(m_CurrentCount + 1) * m_PieceDegree
      + m_StartAngle;

  GLdouble x1 = m_Radius * std::cos(pieceStartAngle);
  GLdouble y1 = m_Radius * std::sin(pieceStartAngle);
  Vertex v1 = m_CenterPoint + ArrangeAxisValues(x1, y1, m_HalfHeight);

  GLdouble x3 = m_Radius * std::cos(pieceEndAngle);
  GLdouble y3 = m_Radius * std::sin(pieceEndAngle);
  Vertex v3 = m_CenterPoint + ArrangeAxisValues(x3, y3, -m_HalfHeight);

  if (m_UpTriangle) {
    Vertex v2 = m_CenterPoint + ArrangeAxisValues(x3, y3, m_HalfHeight);

    m_UpTriangle = false;
    return { v1, v2, v3 };
  } else {
    Vertex v2 = m_CenterPoint + ArrangeAxisValues(x1, y1, -m_HalfHeight);

    m_UpTriangle = true;
    m_CurrentCount += 1;
    return { v1, v3, v2 };
  }
}

Vertex CylinderGenerator::ArrangeAxisValues(GLdouble fanX,
                                            GLdouble fanY,
                                            GLdouble height) {
  switch (m_Axis) {
  case CircleAxis::XAxis:
    return Vertex(height, fanX, fanY);
  case CircleAxis::YAxis:
    return Vertex(fanX, height, fanY);
  case CircleAxis::ZAxis:
    return Vertex(fanX, fanY, height);
  }
}

static Vector PickHeightVector(CircleAxis axis, GLdouble height) {
  switch (axis) {
  case CircleAxis::XAxis:
    return Vector(height, 0.0, 0.0);
  case CircleAxis::YAxis:
    return Vector(0.0, height, 0.0);
  case CircleAxis::ZAxis:
    return Vector(0.0, 0.0, height);
  }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ArgumentSelectionDefects"
std::unique_ptr<TriangleGenerator>
CreateClosedCylinder(const Vector &centerPoint,
                     GLdouble radius,
                     GLdouble height,
                     GLdouble startAngle,
                     GLdouble endAngle,
                     std::size_t count,
                     CircleAxis axis) {
  CylinderGenerator *base = new CylinderGenerator(centerPoint,
                                                  radius,
                                                  height,
                                                  startAngle,
                                                  endAngle,
                                                  count,
                                                  axis);
  Vector heightVector = PickHeightVector(axis, height / 2.0);
  FanGenerator *topSurface = new FanGenerator(centerPoint + heightVector,
                                              radius,
                                              startAngle,
                                              endAngle,
                                              count,
                                              axis);
  FanGenerator *bottomSurface = new FanGenerator(centerPoint - heightVector,
                                                 radius,
                                                 endAngle,
                                                 startAngle,
                                                 count,
                                                 axis);

  std::vector<std::unique_ptr<TriangleGenerator>> triangleGenerators;
  triangleGenerators.push_back(std::unique_ptr<TriangleGenerator>(base));
  triangleGenerators.push_back(std::unique_ptr<TriangleGenerator>(topSurface));
  triangleGenerators.push_back(std::unique_ptr<TriangleGenerator>(bottomSurface));

  return std::unique_ptr<TriangleGenerator>(
      new Composer(std::move(triangleGenerators))
  );
}
#pragma clang diagnostic pop

} // namespace cw
