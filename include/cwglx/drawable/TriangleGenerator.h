#ifndef PROJECT_WG_TRIANGLE_GENERATOR_H
#define PROJECT_WG_TRIANGLE_GENERATOR_H

#include <array>
#include <memory>
#include <vector>
#include "cwglx/Defs.h"
#include "cwglx/Vertex.h"
#include "util/Derive.h"
#include "util/Sinkrate.h"

namespace cw {

class TriangleGenerator {
public:
  TriangleGenerator();
  virtual ~TriangleGenerator() = 0;

  [[nodiscard]] virtual bool HasNextTriangle() = 0;
  [[nodiscard]] virtual std::array<Vertex, 3> NextTriangle() = 0;
  [[nodiscard]] virtual std::unique_ptr<TriangleGenerator> Clone() const = 0;
  virtual void Reset() = 0;

  CW_DERIVE_UNCOPYABLE(TriangleGenerator)
};

class SimpleTriangle final : public TriangleGenerator {
public:
  explicit SimpleTriangle(const std::array<Vertex, 3>& vertices);

  ~SimpleTriangle() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  std::array<Vertex, 3> m_Vertices;
  bool m_Generated;
};

class StoredTriangles final : public TriangleGenerator {
public:
  explicit StoredTriangles(std::vector<std::array<Vertex, 3>>&& triangles);

  explicit
  StoredTriangles(std::shared_ptr<std::vector<std::array<Vertex, 3>>> triangles,
                  const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~StoredTriangles() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  std::shared_ptr<std::vector<std::array<Vertex, 3>>> m_Triangles;
  typename std::vector<std::array<Vertex, 3>>::iterator m_CurrentTriangle;
};

class Positioner final : public TriangleGenerator {
public:
  Positioner(std::unique_ptr<TriangleGenerator> &&generator,
             const Vector& position);

  ~Positioner() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

  void ResetPosition(const Vector& position);

private:
  std::unique_ptr<TriangleGenerator> m_Generator;
  Vector m_Position;
};

class Rotator final : public TriangleGenerator {
public:
  Rotator(std::unique_ptr<TriangleGenerator> &&base,
          const Vertex &centerPoint,
          CircleAxis m_Axis,
          GLdouble m_Degree);

  ~Rotator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  std::unique_ptr<TriangleGenerator> m_Base;
  Vertex m_CenterPoint;
  CircleAxis m_Axis;
  GLdouble m_Degree;
};

class Composer final : public TriangleGenerator {
public:
  explicit
  Composer(std::vector<std::unique_ptr<TriangleGenerator>> &&generators);

  Composer(std::shared_ptr<std::vector<std::unique_ptr<TriangleGenerator>>> ptr,
           const SecretInternalsDoNotUseOrYouWillBeFired &);

  ~Composer() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  std::shared_ptr<std::vector<std::unique_ptr<TriangleGenerator>>> m_Generators;
  std::vector<std::unique_ptr<TriangleGenerator>>::iterator m_CurrentGenerator;
};

class FanGenerator final : public TriangleGenerator {
public:
  FanGenerator(const Vector& centerPoint,
               GLdouble radius,
               GLdouble startAngle,
               GLdouble endAngle,
               std::size_t count);

  FanGenerator(const Vector &centerPoint,
               GLdouble radius,
               GLdouble startAngleRad,
               std::size_t count,
               GLdouble pieceDegreeRad,
               const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~FanGenerator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  Vector m_CenterPoint;
  GLdouble m_Radius;
  GLdouble m_StartAngle;
  std::size_t m_Count;

  GLdouble m_PieceDegree;
  std::size_t m_CurrentCount;
};

class CylinderGenerator final : public TriangleGenerator {
public:
  CylinderGenerator(const Vector &centerPoint,
                    GLdouble radius,
                    GLdouble height,
                    GLdouble startAngle,
                    GLdouble endAngle,
                    std::size_t count);

  CylinderGenerator(const Vector &centerPoint,
                    GLdouble radius,
                    GLdouble halfHeight,
                    GLdouble startAngleRad,
                    std::size_t count,
                    GLdouble pieceDegreeRad,
                    const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~CylinderGenerator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  Vector m_CenterPoint;
  GLdouble m_Radius;
  GLdouble m_HalfHeight;
  GLdouble m_StartAngle;
  std::size_t m_Count;

  GLdouble m_PieceDegree;
  std::size_t m_CurrentCount;
  bool m_UpTriangle;
};

std::unique_ptr<TriangleGenerator>
CreateClosedCylinder(const Vector &centerPoint,
                     GLdouble radius,
                     GLdouble height,
                     GLdouble startAngle,
                     GLdouble endAngle,
                     std::size_t count);

class SphereGenerator final : public TriangleGenerator {
public:
  SphereGenerator(const Vector &centerPoint,
                  GLdouble radius,
                  GLdouble xyStartAngle,
                  GLdouble xyEndAngle,
                  GLdouble zStartAngle,
                  GLdouble zEndAngle,
                  std::size_t xyCount,
                  std::size_t zCount);

  SphereGenerator(const Vector &centerPoint,
                  GLdouble radius,
                  GLdouble xyStartAngleRad,
                  GLdouble zStartAngleRad,
                  std::size_t xyCount,
                  std::size_t zCount,
                  GLdouble xyPieceDegreeRad,
                  GLdouble zPieceDegreeRad,
                  const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~SphereGenerator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  Vector m_CenterPoint;
  GLdouble m_Radius;
  GLdouble m_XYStartAngle;
  GLdouble m_ZStartAngle;
  std::size_t m_XYCount;
  std::size_t m_ZCount;
  GLdouble m_XYPieceDegree;
  GLdouble m_ZPieceDegree;

  std::size_t m_CurrentXYCount;
  std::size_t m_CurrentZCount;
  bool m_UpTriangle;
};

class DonutGenerator final : public TriangleGenerator {
public:
  DonutGenerator(const Vector &centerPosition,
                 GLdouble radius,
                 GLdouble pipeRadius,
                 GLdouble startAngle,
                 GLdouble endAngle,
                 std::size_t pipeCount,
                 std::size_t pipePolyCount);

  DonutGenerator(const Vector &centerPosition,
                 GLdouble radius,
                 GLdouble pipeRadius,
                 GLdouble startAngleRad,
                 std::size_t pipeCount,
                 std::size_t pipePolyCount,
                 GLdouble pieceDegreeRad,
                 GLdouble piecePolyDegreeRad,
                 const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~DonutGenerator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  Vector m_CenterPoint;
  GLdouble m_Radius;
  GLdouble m_PipeRadius;
  GLdouble m_StartAngle;
  std::size_t m_PipeCount;
  std::size_t m_PipePolyCount;
  GLdouble m_PieceDegree;
  GLdouble m_PiecePolyDegree;

  std::size_t m_CurrentPipeCount;
  std::size_t m_CurrentPipePolyCount;
  bool m_UpTriangle;
};

class BoxGenerator final : public TriangleGenerator {
public:
  BoxGenerator(const Vector &centerPoint,
               GLdouble xScale,
               GLdouble yScale,
               GLdouble zScale);

  BoxGenerator(const Vector &centerPoint,
               GLdouble xScale,
               GLdouble yScale,
               GLdouble zScale,
               const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~BoxGenerator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  Vector m_CenterPoint;
  GLdouble m_XScale;
  GLdouble m_YScale;
  GLdouble m_ZScale;

  std::size_t m_CurrentCount;
};

class ConvexPolyGenerator final : public TriangleGenerator {
public:
  explicit ConvexPolyGenerator(std::vector<Vertex> &&vertices);

  ConvexPolyGenerator(std::shared_ptr<std::vector<Vertex>> vertices,
                      const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~ConvexPolyGenerator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] std::array<Vertex, 3> NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGenerator> Clone() const final;

  void Reset() final;

private:
  std::shared_ptr<std::vector<Vertex>> m_Vertices;
  std::size_t m_CurrentCount;
};

} // namespace cw

#endif // PROJECT_WG_TRIANGLE_GENERATOR_H
