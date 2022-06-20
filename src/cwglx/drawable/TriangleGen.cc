#include "cwglx/drawable/TriangleGen.h"

#include "cwglx/drawable/PlainTriangles.h"
#include "util/Constants.h"

namespace cw {

TriangleGen::~TriangleGen() = default;

SimpleTriangle::SimpleTriangle(const Triangle& vertices)
  : m_Vertices(vertices),
    m_Generated(false)
{}

SimpleTriangle::~SimpleTriangle() = default;

bool SimpleTriangle::HasNextTriangle() {
  return !m_Generated;
}

Triangle SimpleTriangle::NextTriangle() {
  m_Generated = true;
  return m_Vertices;
}

std::unique_ptr<TriangleGen> SimpleTriangle::Clone() const {
  return std::make_unique<SimpleTriangle>(m_Vertices);
}

void SimpleTriangle::Reset() {
  m_Generated = false;
}

StoredTriangles::StoredTriangles(std::vector<Triangle> &&triangles)
  : m_Triangles(std::make_shared<std::vector<Triangle>>(
      std::move(triangles)
    )),
    m_CurrentTriangle(m_Triangles->begin())
{}

StoredTriangles::
StoredTriangles(std::shared_ptr<std::vector<Triangle>> triangles,
                const SecretInternalsDoNotUseOrYouWillBeFired &)
  : m_Triangles(std::move(triangles)),
    m_CurrentTriangle(m_Triangles->begin())
{}

StoredTriangles::~StoredTriangles() = default;

bool StoredTriangles::HasNextTriangle() {
  return m_CurrentTriangle < m_Triangles->end();
}

Triangle StoredTriangles::NextTriangle() {
  Triangle triangle = *m_CurrentTriangle;
  m_CurrentTriangle++;
  return triangle;
}

std::unique_ptr<TriangleGen> StoredTriangles::Clone() const {
  return std::make_unique<StoredTriangles>(
      m_Triangles,
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

void StoredTriangles::Reset() {
  m_CurrentTriangle = m_Triangles->begin();
}

Positioner::Positioner(std::unique_ptr<TriangleGen> &&generator,
                       const Vector& position)
  : m_Generator(std::move(generator)),
    m_Position(position)
{}

Positioner::~Positioner() = default;

bool Positioner::HasNextTriangle() {
  return m_Generator->HasNextTriangle();
}

Triangle Positioner::NextTriangle() {
  Triangle triangle = m_Generator->NextTriangle();
  for (Vertex& vertex : triangle) {
    vertex += m_Position;
  }
  return triangle;
}

std::unique_ptr<TriangleGen> Positioner::Clone() const {
  return std::make_unique<Positioner>(m_Generator->Clone(), m_Position);
}

void Positioner::Reset() {
  m_Generator->Reset();
}

void Positioner::ResetPosition(const Vector &position) {
  m_Position = position;
  Reset();
}

Rotator::Rotator(std::unique_ptr<TriangleGen> &&base,
                 const Vertex &centerPoint,
                 Axis axis,
                 GLdouble degree)
  : m_Base(std::move(base)),
    m_CenterPoint(centerPoint),
    m_Axis(axis),
    m_Degree(DegToRad(degree))
{}

Rotator::Rotator(std::unique_ptr<TriangleGen> &&base,
                 const Vertex &centerPoint,
                 Axis axis,
                 GLdouble rad,
                 const SecretInternalsDoNotUseOrYouWillBeFired&)
  : m_Base(std::move(base)),
    m_CenterPoint(centerPoint),
    m_Axis(axis),
    m_Degree(rad)
{}

Rotator::~Rotator() = default;

bool Rotator::HasNextTriangle() {
  return m_Base->HasNextTriangle();
}

std::unique_ptr<TriangleGen> Rotator::Clone() const {
  return std::make_unique<Rotator>(
      m_Base->Clone(),
      m_CenterPoint,
      m_Axis,
      m_Degree,
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

Triangle Rotator::NextTriangle() {
  Triangle triangle = m_Base->NextTriangle();
  for (Vertex &vertex : triangle) {
    vertex = RotateVertex(vertex, m_CenterPoint, m_Axis, m_Degree);
  }
  return triangle;
}

void Rotator::Reset() {
  m_Base->Reset();
}

Composer::Composer(std::vector<std::unique_ptr<TriangleGen>> &&generators)
  : Composer(
      std::make_shared<std::vector<std::unique_ptr<TriangleGen>>>(
          std::move(generators)
      ),
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
    )
{}

Composer::
Composer(std::shared_ptr<std::vector<std::unique_ptr<TriangleGen>>> ptr,
         const SecretInternalsDoNotUseOrYouWillBeFired&)
  : m_Generators(std::move(ptr)),
    m_CurrentGenerator(m_Generators->begin())
{}

Composer::~Composer() = default;

bool Composer::HasNextTriangle() {
  while (m_CurrentGenerator != m_Generators->cend()
         && !m_CurrentGenerator->get()->HasNextTriangle()) {
    ++m_CurrentGenerator;
  }

  if (m_CurrentGenerator != m_Generators->cend()) {
    return true;
  } else {
    Reset();
    return false;
  }
}

Triangle Composer::NextTriangle() {
  while (!m_CurrentGenerator->get()->HasNextTriangle()) {
    ++m_CurrentGenerator;
  }

  return m_CurrentGenerator->get()->NextTriangle();
}

std::unique_ptr<TriangleGen> Composer::Clone() const {
  return std::make_unique<Composer>(
      m_Generators,
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

void Composer::Reset() {
  for (auto& generator : *m_Generators) {
    generator->Reset();
  }
  m_CurrentGenerator = m_Generators->begin();
}

FanGenerator::FanGenerator(const Vector &centerPoint,
                           GLdouble radius,
                           GLdouble startAngle,
                           GLdouble endAngle,
                           std::size_t count)
  : FanGenerator(centerPoint,
                 radius,
                 DegToRad(startAngle),
                 count,
                 DegToRad(endAngle - startAngle) / static_cast<GLdouble>(count),
                 SecretInternalsDoNotUseOrYouWillBeFired::Instance)
{}

FanGenerator::FanGenerator(const Vector &centerPoint,
                           GLdouble radius,
                           GLdouble startAngleRad,
                           std::size_t count,
                           GLdouble pieceDegreeRad,
                           const SecretInternalsDoNotUseOrYouWillBeFired&)
  : m_CenterPoint(centerPoint),
    m_Radius(radius),
    m_StartAngle(startAngleRad),
    m_Count(count),
    m_PieceDegree(pieceDegreeRad),
    m_CurrentCount(0)
{}

FanGenerator::~FanGenerator() = default;

bool FanGenerator::HasNextTriangle() {
  return m_CurrentCount < m_Count;
}

Triangle FanGenerator::NextTriangle() {
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

std::unique_ptr<TriangleGen> FanGenerator::Clone() const {
  return std::make_unique<FanGenerator>(
      m_CenterPoint,
      m_Radius,
      m_StartAngle,
      m_Count,
      m_PieceDegree,
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

void FanGenerator::Reset() {
  m_CurrentCount = 0;
}

CylinderGenerator::CylinderGenerator(const Vector &centerPoint,
                                     GLdouble radius,
                                     GLdouble height,
                                     GLdouble startAngle,
                                     GLdouble endAngle,
                                     std::size_t count)
  : CylinderGenerator(
      centerPoint,
      radius,
      height / 2.0,
      DegToRad(startAngle),
      count,
      DegToRad(endAngle - startAngle) / static_cast<GLdouble>(count),
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
    )
{}

CylinderGenerator::
CylinderGenerator(const Vector &centerPoint,
                  GLdouble radius,
                  GLdouble halfHeight,
                  GLdouble startAngleRad,
                  std::size_t count,
                  GLdouble pieceDegreeRad,
                  const SecretInternalsDoNotUseOrYouWillBeFired &)
  : m_CenterPoint(centerPoint),
    m_Radius(radius),
    m_HalfHeight(halfHeight),
    m_StartAngle(startAngleRad),
    m_Count(count),
    m_PieceDegree(pieceDegreeRad),
    m_CurrentCount(0),
    m_UpTriangle(true)
{}

CylinderGenerator::~CylinderGenerator() = default;

bool CylinderGenerator::HasNextTriangle() {
  return m_CurrentCount < m_Count;
}

Triangle CylinderGenerator::NextTriangle() {
  Q_ASSERT(HasNextTriangle());

  GLdouble pieceStartAngle =
      static_cast<GLdouble>(m_CurrentCount) * m_PieceDegree
      + m_StartAngle;
  GLdouble pieceEndAngle =
      static_cast<GLdouble>(m_CurrentCount + 1) * m_PieceDegree
      + m_StartAngle;

  GLdouble x1 = m_Radius * std::cos(pieceStartAngle);
  GLdouble y1 = m_Radius * std::sin(pieceStartAngle);
  Vertex v1 = m_CenterPoint + Vertex(x1, y1, m_HalfHeight);

  GLdouble x3 = m_Radius * std::cos(pieceEndAngle);
  GLdouble y3 = m_Radius * std::sin(pieceEndAngle);
  Vertex v3 = m_CenterPoint + Vertex(x3, y3, -m_HalfHeight);

  if (m_UpTriangle) {
    Vertex v2 = m_CenterPoint + Vertex(x1, y1, -m_HalfHeight);

    m_UpTriangle = false;
    return { v1, v2, v3 };
  } else {
    Vertex v2 = m_CenterPoint + Vertex(x3, y3, m_HalfHeight);

    m_UpTriangle = true;
    m_CurrentCount += 1;
    return { v1, v3, v2 };
  }
}

std::unique_ptr<TriangleGen> CylinderGenerator::Clone() const {
  return std::make_unique<CylinderGenerator>(
      m_CenterPoint,
      m_Radius,
      m_HalfHeight,
      m_StartAngle,
      m_Count,
      m_PieceDegree,
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

void CylinderGenerator::Reset() {
  m_CurrentCount = 0;
  m_UpTriangle = true;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ArgumentSelectionDefects"
std::unique_ptr<TriangleGen>
CreateClosedCylinder(const Vector &centerPoint,
                     GLdouble radius,
                     GLdouble height,
                     GLdouble startAngle,
                     GLdouble endAngle,
                     std::size_t count) {
  CylinderGenerator *base = new CylinderGenerator(centerPoint,
                                                  radius,
                                                  height,
                                                  startAngle,
                                                  endAngle,
                                                  count);
  Vector heightVector = Vector(0.0, 0.0, height / 2.0);
  FanGenerator *topSurface = new FanGenerator(centerPoint + heightVector,
                                              radius,
                                              startAngle,
                                              endAngle,
                                              count);
  FanGenerator *bottomSurface = new FanGenerator(centerPoint - heightVector,
                                                 radius,
                                                 endAngle,
                                                 startAngle,
                                                 count);

  std::vector<std::unique_ptr<TriangleGen>> triangleGenerators;
  triangleGenerators.push_back(std::unique_ptr<TriangleGen>(base));
  triangleGenerators.push_back(std::unique_ptr<TriangleGen>(topSurface));
  triangleGenerators.push_back(std::unique_ptr<TriangleGen>(bottomSurface));

  return std::unique_ptr<TriangleGen>(
      new Composer(std::move(triangleGenerators))
  );
}
#pragma clang diagnostic pop

SphereGenerator::SphereGenerator(const Vector &centerPoint,
                                 GLdouble radius,
                                 GLdouble xyStartAngle,
                                 GLdouble xyEndAngle,
                                 GLdouble zStartAngle,
                                 GLdouble zEndAngle,
                                 std::size_t xyCount,
                                 std::size_t zCount)
  : SphereGenerator(centerPoint,
                    radius,
                    DegToRad(xyStartAngle),
                    DegToRad(zStartAngle),
                    xyCount,
                    zCount,
                    DegToRad(xyEndAngle - xyStartAngle)
                      / static_cast<GLdouble>(xyCount),
                    DegToRad(zEndAngle - zStartAngle)
                      / static_cast<GLdouble>(zCount),
                    SecretInternalsDoNotUseOrYouWillBeFired::Instance)
{
  Q_ASSERT(xyStartAngle >= 0.0 && "Only first octant is ok");
  Q_ASSERT(xyEndAngle <= 90.0 && "Only first octant is ok");
  Q_ASSERT(zStartAngle >= 0.0 && "Only first octant is ok");
  Q_ASSERT(zEndAngle <= 90.0 && "Only first octant is ok");
}

SphereGenerator::SphereGenerator(const Vector &centerPoint,
                                 GLdouble radius,
                                 GLdouble xyStartAngleRad,
                                 GLdouble zStartAngleRad,
                                 std::size_t xyCount,
                                 std::size_t zCount,
                                 GLdouble xyPieceDegreeRad,
                                 GLdouble zPieceDegreeRad,
                                 const SecretInternalsDoNotUseOrYouWillBeFired&)
  : m_CenterPoint(centerPoint),
    m_Radius(radius),
    m_XYStartAngle(xyStartAngleRad),
    m_ZStartAngle(zStartAngleRad),
    m_XYCount(xyCount),
    m_ZCount(zCount),
    m_XYPieceDegree(xyPieceDegreeRad),
    m_ZPieceDegree(zPieceDegreeRad),

    m_CurrentXYCount(0),
    m_CurrentZCount(0),
    m_UpTriangle(true)
{}

SphereGenerator::~SphereGenerator() = default;

bool SphereGenerator::HasNextTriangle() {
  return m_CurrentXYCount < m_XYCount && m_CurrentZCount < m_ZCount;
}

Triangle SphereGenerator::NextTriangle() {
  Q_ASSERT(HasNextTriangle());

  GLdouble zStartAngle =
      m_ZStartAngle
      + static_cast<GLdouble>(m_CurrentZCount) * m_ZPieceDegree;
  GLdouble zEndAngle =
      m_ZStartAngle
      + static_cast<GLdouble>(m_CurrentZCount + 1) * m_ZPieceDegree;

  GLdouble xyStartAngle =
      m_XYStartAngle
      + static_cast<GLdouble>(m_CurrentXYCount) * m_XYPieceDegree;
  GLdouble xyEndAngle =
      m_XYStartAngle
      + static_cast<GLdouble>(m_CurrentXYCount + 1) * m_XYPieceDegree;

  GLdouble z0 = m_Radius * std::sin(zStartAngle);
  GLdouble z0r = m_Radius * std::cos(zStartAngle);
  GLdouble z1 = m_Radius * std::sin(zEndAngle);
  GLdouble z1r = m_Radius * std::cos(zEndAngle);

  GLdouble x01 = z0r * std::cos(xyStartAngle);
  GLdouble y01 = z0r * std::sin(xyStartAngle);
  GLdouble x02 = z0r * std::cos(xyEndAngle);
  GLdouble y02 = z0r * std::sin(xyEndAngle);

  GLdouble x11 = z1r * std::cos(xyStartAngle);
  GLdouble y11 = z1r * std::sin(xyStartAngle);
  GLdouble x12 = z1r * std::cos(xyEndAngle);
  GLdouble y12 = z1r * std::sin(xyEndAngle);

  Vertex v1(x01, y01, z0);
  Vertex v3(x12, y12, z1);

  if (m_UpTriangle) {
    Vertex v2(x02, y02, z0);
    m_UpTriangle = false;
    return {v1 + m_CenterPoint, v2 + m_CenterPoint, v3 + m_CenterPoint};
  } else {
    Vertex v2(x11, y11, z1);
    m_UpTriangle = true;
    m_CurrentXYCount += 1;
    if (m_CurrentXYCount == m_XYCount) {
      m_CurrentXYCount = 0;
      m_CurrentZCount += 1;
    }
    return {v1 + m_CenterPoint, v3 + m_CenterPoint, v2 + m_CenterPoint};
  }
}

std::unique_ptr<TriangleGen> SphereGenerator::Clone() const {
  return std::make_unique<SphereGenerator>(
      m_CenterPoint,
      m_Radius,
      m_XYStartAngle,
      m_ZStartAngle,
      m_XYCount,
      m_ZCount,
      m_XYPieceDegree,
      m_ZPieceDegree,
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

void SphereGenerator::Reset() {
  m_CurrentXYCount = 0;
  m_CurrentZCount = 0;
  m_UpTriangle = true;
}

DonutGenerator::DonutGenerator(const Vector &centerPoint,
                               GLdouble radius,
                               GLdouble pipeRadius,
                               GLdouble startAngle,
                               GLdouble endAngle,
                               std::size_t pipeCount,
                               std::size_t pipePolyCount)
  : DonutGenerator(centerPoint,
                   radius + pipeRadius / 2.0,
                   pipeRadius,
                   DegToRad(startAngle),
                   pipeCount,
                   pipePolyCount,
                   DegToRad(endAngle - startAngle)
                   / static_cast<GLdouble>(pipeCount),
                   constants::PI * 2 / static_cast<GLdouble>(pipePolyCount),
                   SecretInternalsDoNotUseOrYouWillBeFired::Instance)
{}

DonutGenerator::DonutGenerator(const Vector &centerPoint,
                               GLdouble radius,
                               GLdouble pipeRadius,
                               GLdouble startAngleRad,
                               std::size_t pipeCount,
                               std::size_t pipePolyCount,
                               GLdouble pieceDegreeRad,
                               GLdouble piecePolyDegreeRad,
                               const SecretInternalsDoNotUseOrYouWillBeFired &)
  : m_CenterPoint(centerPoint),
    m_Radius(radius),
    m_PipeRadius(pipeRadius),
    m_StartAngle(startAngleRad),
    m_PipeCount(pipeCount),
    m_PipePolyCount(pipePolyCount),
    m_PieceDegree(pieceDegreeRad),
    m_PiecePolyDegree(piecePolyDegreeRad),

    m_CurrentPipeCount(0),
    m_CurrentPipePolyCount(0),
    m_UpTriangle(true)
{}

DonutGenerator::~DonutGenerator() = default;

bool DonutGenerator::HasNextTriangle() {
  return m_CurrentPipeCount < m_PipeCount
         && m_CurrentPipePolyCount < m_PipePolyCount;
}

Triangle DonutGenerator::NextTriangle() {
  Q_ASSERT(HasNextTriangle());

  GLdouble yStartAngle =
      m_StartAngle
      + static_cast<GLdouble>(m_CurrentPipeCount) * m_PieceDegree;
  GLdouble yEndAngle =
      m_StartAngle
      + static_cast<GLdouble>(m_CurrentPipeCount + 1) * m_PieceDegree;

  GLdouble xzStartAngle =
      m_StartAngle
      + static_cast<GLdouble>(m_CurrentPipePolyCount) * m_PiecePolyDegree;
  GLdouble xzEndAngle =
      m_StartAngle
      + static_cast<GLdouble>(m_CurrentPipePolyCount + 1) * m_PiecePolyDegree;

  GLdouble x0 = m_PipeRadius * std::cos(xzStartAngle);
  GLdouble z0 = m_PipeRadius * std::sin(xzStartAngle);
  GLdouble x1 = m_PipeRadius * std::cos(xzEndAngle);
  GLdouble z1 = m_PipeRadius * std::sin(xzEndAngle);

  Vertex v0 = Vertex(x0 + m_Radius, 0, z0);
  Vertex v1 = Vertex(x1 + m_Radius, 0, z1);

  Vertex v01 = RotateVertex(v0,
                            constants::g_ZeroVertex,
                            Axis::ZAxis,
                            yStartAngle);
  Vertex v11 = RotateVertex(v1,
                            constants::g_ZeroVertex,
                            Axis::ZAxis,
                            yStartAngle);
  Vertex v02 = RotateVertex(v0,
                            constants::g_ZeroVertex,
                            Axis::ZAxis,
                            yEndAngle);
  Vertex v12 = RotateVertex(v1,
                            constants::g_ZeroVertex,
                            Axis::ZAxis,
                            yEndAngle);

  if (m_UpTriangle) {
    m_UpTriangle = false;

    return {
        v01 + m_CenterPoint,
        v12 + m_CenterPoint,
        v11 + m_CenterPoint
    };
  } else {
    m_UpTriangle = true;
    m_CurrentPipeCount += 1;
    if (m_CurrentPipeCount == m_PipeCount) {
      m_CurrentPipeCount = 0;
      m_CurrentPipePolyCount += 1;
    }

    return {
        v01 + m_CenterPoint,
        v02 + m_CenterPoint,
        v12 + m_CenterPoint
    };
  }
}

std::unique_ptr<TriangleGen> DonutGenerator::Clone() const {
  return std::make_unique<DonutGenerator>(
      m_CenterPoint,
      m_Radius,
      m_PipeRadius,
      m_StartAngle,
      m_PipeCount,
      m_PipePolyCount,
      m_PieceDegree,
      m_PiecePolyDegree,
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

void DonutGenerator::Reset() {
  m_CurrentPipeCount = 0;
  m_CurrentPipePolyCount = 0;
  m_UpTriangle = true;
}

BoxGenerator::BoxGenerator(const Vector &centerPoint,
                           GLdouble xScale,
                           GLdouble yScale,
                           GLdouble zScale)
  : BoxGenerator(centerPoint,
                 xScale / 2.0,
                 yScale / 2.0,
                 zScale / 2.0,
                 SecretInternalsDoNotUseOrYouWillBeFired::Instance)
{}

BoxGenerator::BoxGenerator(const Vector &centerPoint,
                           GLdouble xScale,
                           GLdouble yScale,
                           GLdouble zScale,
                           const SecretInternalsDoNotUseOrYouWillBeFired &)
  : m_CenterPoint(centerPoint),
    m_XScale(xScale),
    m_YScale(yScale),
    m_ZScale(zScale),
    m_CurrentCount(0)
{}

BoxGenerator::~BoxGenerator() = default;

[[nodiscard]] bool BoxGenerator::HasNextTriangle() {
  return m_CurrentCount < 12;
}

[[nodiscard]] Triangle BoxGenerator::NextTriangle() {
  Q_ASSERT(HasNextTriangle());

  m_CurrentCount += 1;
  switch (m_CurrentCount - 1) {
    // front face, z = +Z
    case 0: return {
        m_CenterPoint + Vertex(-m_XScale, m_YScale, m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, m_ZScale),
        m_CenterPoint + Vertex(m_XScale, -m_YScale, m_ZScale)
    };
    case 1: return {
        m_CenterPoint + Vertex(-m_XScale, m_YScale, m_ZScale),
        m_CenterPoint + Vertex(m_XScale, -m_YScale, m_ZScale),
        m_CenterPoint + Vertex(m_XScale, m_YScale, m_ZScale)
    };
    // back face, z = -Z
    case 2: return {
        m_CenterPoint + Vertex(m_XScale, m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(m_XScale, -m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, -m_ZScale)
    };
    case 3: return {
        m_CenterPoint + Vertex(m_XScale, m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, m_YScale, -m_ZScale)
    };
    // right face, x = X
    case 4: return {
        m_CenterPoint + Vertex(m_XScale, m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(m_XScale, -m_YScale, m_ZScale),
        m_CenterPoint + Vertex(m_XScale, -m_YScale, -m_ZScale)
    };
    case 5: return {
        m_CenterPoint + Vertex(m_XScale, m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(m_XScale, m_YScale, m_ZScale),
        m_CenterPoint + Vertex(m_XScale, -m_YScale, m_ZScale)
    };
    // left face, x = -X
    case 6: return {
        m_CenterPoint + Vertex(-m_XScale, m_YScale, m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, m_ZScale)
    };
    case 7: return {
        m_CenterPoint + Vertex(-m_XScale, m_YScale, m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, -m_ZScale)
    };
    // top face, y = +Y
    case 8: return {
        m_CenterPoint + Vertex(-m_XScale, m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, m_YScale, m_ZScale),
        m_CenterPoint + Vertex(m_XScale, m_YScale, m_ZScale)
    };
    case 9: return {
        m_CenterPoint + Vertex(-m_XScale, m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(m_XScale, m_YScale, m_ZScale),
        m_CenterPoint + Vertex(m_XScale, m_YScale, -m_ZScale)
    };
    // bottom face, y = -Y
    case 10: return {
        m_CenterPoint + Vertex(m_XScale, -m_YScale, m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, -m_ZScale),
        m_CenterPoint + Vertex(m_XScale, -m_YScale, -m_ZScale)
    };
    case 11: return {
        m_CenterPoint + Vertex(m_XScale, -m_YScale, m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, m_ZScale),
        m_CenterPoint + Vertex(-m_XScale, -m_YScale, -m_ZScale)
    };

    default:
      Q_UNREACHABLE();
  }
}

std::unique_ptr<TriangleGen> BoxGenerator::Clone() const {
  return std::make_unique<BoxGenerator>(
      m_CenterPoint,
      m_XScale,
      m_YScale,
      m_ZScale,
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

void BoxGenerator::Reset() {
  m_CurrentCount = 0;
}

ConvexPolyGenerator::ConvexPolyGenerator(std::vector<Vertex> &&vertices)
    : ConvexPolyGenerator(
    std::make_shared<std::vector<Vertex>>(std::move(vertices)),
    SecretInternalsDoNotUseOrYouWillBeFired::Instance
)
{}

ConvexPolyGenerator::
ConvexPolyGenerator(std::shared_ptr<std::vector<Vertex>> vertices,
                    const SecretInternalsDoNotUseOrYouWillBeFired &)
    : m_Vertices(std::move(vertices)),
      m_CurrentCount(1)
{}

ConvexPolyGenerator::~ConvexPolyGenerator() = default;

bool ConvexPolyGenerator::HasNextTriangle() {
  return m_CurrentCount < m_Vertices->size() - 1;
}

Triangle ConvexPolyGenerator::NextTriangle() {
  Q_ASSERT(HasNextTriangle());

  const std::size_t i0 = m_CurrentCount;
  const std::size_t i1 = i0 + 1;
  m_CurrentCount++;

  Vertex origin = m_Vertices->operator[](0);
  Vertex v0 = m_Vertices->operator[](i0);
  Vertex v1 = m_Vertices->operator[](i1);

  return { origin, v0, v1 };
}

std::unique_ptr<TriangleGen> ConvexPolyGenerator::Clone() const {
  return std::make_unique<ConvexPolyGenerator>(
      std::make_shared<std::vector<Vertex>>(*m_Vertices),
      SecretInternalsDoNotUseOrYouWillBeFired::Instance
  );
}

void ConvexPolyGenerator::Reset() {
  m_CurrentCount = 1;
}

Flipper::Flipper(std::unique_ptr<TriangleGen> &&generator, Plane plane)
  : m_Generator(std::move(generator)),
    m_Plane(plane)
{}

Flipper::~Flipper() = default;

bool Flipper::HasNextTriangle() {
  return m_Generator->HasNextTriangle();
}

Triangle Flipper::NextTriangle() {
  auto [v1, v2, v3] = m_Generator->NextTriangle();
  switch (m_Plane) {
    case Plane::XYPlane:
      v1.SetZ(-v1.GetZ());
      v2.SetZ(-v2.GetZ());
      v3.SetZ(-v3.GetZ());
      break;

    case Plane::XZPlane:
      v1.SetY(-v1.GetY());
      v2.SetY(-v2.GetY());
      v3.SetY(-v3.GetY());
      break;

    case Plane::YZPlane:
      v1.SetX(-v1.GetX());
      v2.SetX(-v2.GetX());
      v3.SetX(-v3.GetX());
      break;

    default:
      Q_UNREACHABLE();
  }

  return { v1, v3, v2 };
}

std::unique_ptr<TriangleGen> Flipper::Clone() const {
  return std::make_unique<Flipper>(m_Generator->Clone(),
                                   m_Plane);
}

void Flipper::Reset() {
  m_Generator->Reset();
}

Inverter::Inverter(std::unique_ptr<TriangleGen> &&generator)
  : m_Generator(std::move(generator))
{}

Inverter::~Inverter() = default;

bool Inverter::HasNextTriangle() {
  return m_Generator->HasNextTriangle();
}

Triangle Inverter::NextTriangle() {
  auto [v1, v2, v3] = m_Generator->NextTriangle();
  return { v1, v3, v2 };
}

std::unique_ptr<TriangleGen> Inverter::Clone() const {
  return std::make_unique<Inverter>(m_Generator->Clone());
}

void Inverter::Reset() {
  m_Generator->Reset();
}

} // namespace cw
