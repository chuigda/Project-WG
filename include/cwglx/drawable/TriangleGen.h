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

class TriangleGen {
public:
  constexpr inline TriangleGen() = default;
  virtual ~TriangleGen() = 0;

  [[nodiscard]] virtual bool HasNextTriangle() = 0;
  [[nodiscard]] virtual Triangle NextTriangle() = 0;
  [[nodiscard]] virtual std::unique_ptr<TriangleGen> Clone() const = 0;
  virtual void Reset() = 0;

  CW_DERIVE_UNCOPYABLE(TriangleGen)
};

class SimpleTriangle final : public TriangleGen {
public:
  explicit SimpleTriangle(const Triangle& vertices);

  ~SimpleTriangle() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  Triangle m_Vertices;
  bool m_Generated;
};

class StoredTriangles final : public TriangleGen {
public:
  explicit StoredTriangles(std::vector<Triangle>&& triangles);

  explicit
  StoredTriangles(std::shared_ptr<std::vector<Triangle>> triangles,
                  const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~StoredTriangles() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  std::shared_ptr<std::vector<Triangle>> m_Triangles;
  typename std::vector<Triangle>::iterator m_CurrentTriangle;
};

class Positioner final : public TriangleGen {
public:
  Positioner(std::unique_ptr<TriangleGen> &&generator,
             const Vector& position);

  ~Positioner() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

  void ResetPosition(const Vector& position);

private:
  std::unique_ptr<TriangleGen> m_Generator;
  Vector m_Position;
};

class Rotator final : public TriangleGen {
public:
  Rotator(std::unique_ptr<TriangleGen> &&base,
          const Vertex &centerPoint,
          Axis axis,
          GLdouble degree);

  Rotator(std::unique_ptr<TriangleGen> &&base,
          const Vertex &centerPoint,
          Axis axis,
          GLdouble rad,
          const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~Rotator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  std::unique_ptr<TriangleGen> m_Base;
  Vertex m_CenterPoint;
  Axis m_Axis;
  GLdouble m_Degree;
};

class Composer final : public TriangleGen {
public:
  explicit
  Composer(std::vector<std::unique_ptr<TriangleGen>> &&generators);

  Composer(std::shared_ptr<std::vector<std::unique_ptr<TriangleGen>>> ptr,
           const SecretInternalsDoNotUseOrYouWillBeFired &);

  ~Composer() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  std::shared_ptr<std::vector<std::unique_ptr<TriangleGen>>> m_Generators;
  std::vector<std::unique_ptr<TriangleGen>>::iterator m_CurrentGenerator;
};

class FanGenerator final : public TriangleGen {
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

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  Vector m_CenterPoint;
  GLdouble m_Radius;
  GLdouble m_StartAngle;
  std::size_t m_Count;

  GLdouble m_PieceDegree;
  std::size_t m_CurrentCount;
};

class CylinderGenerator final : public TriangleGen {
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

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

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

std::unique_ptr<TriangleGen>
CreateClosedCylinder(const Vector &centerPoint,
                     GLdouble radius,
                     GLdouble height,
                     GLdouble startAngle,
                     GLdouble endAngle,
                     std::size_t count);

class SphereGenerator final : public TriangleGen {
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

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

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

class DonutGenerator final : public TriangleGen {
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

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

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

class BoxGenerator final : public TriangleGen {
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

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  Vector m_CenterPoint;
  GLdouble m_XScale;
  GLdouble m_YScale;
  GLdouble m_ZScale;

  std::size_t m_CurrentCount;
};

class ConvexPolyGenerator final : public TriangleGen {
public:
  explicit ConvexPolyGenerator(std::vector<Vertex> &&vertices);

  ConvexPolyGenerator(std::shared_ptr<std::vector<Vertex>> vertices,
                      const SecretInternalsDoNotUseOrYouWillBeFired&);

  ~ConvexPolyGenerator() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  std::shared_ptr<std::vector<Vertex>> m_Vertices;
  std::size_t m_CurrentCount;
};

class Flipper final : public TriangleGen {
public:
  Flipper(std::unique_ptr<TriangleGen> &&generator, Plane plane);

  ~Flipper() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  std::unique_ptr<TriangleGen> m_Generator;
  Plane m_Plane;
};

class Inverter final : public TriangleGen {
public:
  explicit Inverter(std::unique_ptr<TriangleGen> &&generator);

  ~Inverter() final;

  [[nodiscard]] bool HasNextTriangle() final;

  [[nodiscard]] Triangle NextTriangle() final;

  [[nodiscard]] std::unique_ptr<TriangleGen> Clone() const final;

  void Reset() final;

private:
  std::unique_ptr<TriangleGen> m_Generator;
};

} // namespace cw

#endif // PROJECT_WG_TRIANGLE_GENERATOR_H
