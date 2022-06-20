#ifndef PROJECT_WG_FINE_TRIANGLE_GEN_H
#define PROJECT_WG_FINE_TRIANGLE_GEN_H

#include <memory>
#include <utility>

#include "cwglx/Vertex.h"
#include "util/Derive.h"

namespace cw {

class FineTriangleGen {
public:
  constexpr inline FineTriangleGen() = default;
  virtual ~FineTriangleGen() = 0;

  [[nodiscard]] virtual bool HasNextTriangle() = 0;
  [[nodiscard]] virtual std::pair<Triangle, TriangleNormal> NextTriangle() = 0;
  [[nodiscard]] virtual std::unique_ptr<FineTriangleGen> Clone() const = 0;
  virtual void Reset() = 0;

  CW_DERIVE_UNCOPYABLE(FineTriangleGen)
};

} // namespace cw

#endif // PROJECT_WG_FINE_TRIANGLE_GEN_H
