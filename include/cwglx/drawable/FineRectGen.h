#ifndef PROJECT_WG_FINE_RECT_GEN_H
#define PROJECT_WG_FINE_RECT_GEN_H

#include <memory>
#include <utility>

#include "cwglx/Vertex.h"
#include "util/Derive.h"

namespace cw {

class FineRectGen {
public:
  constexpr inline FineRectGen() = default;
  virtual ~FineRectGen() = 0;

  [[nodiscard]] virtual bool HasNextRect() = 0;
  [[nodiscard]] virtual std::pair<Rect, RectNormal> NextRect() = 0;
  [[nodiscard]] virtual std::unique_ptr<FineRectGen> Clone() const = 0;
  virtual void Reset() = 0;

  CW_DERIVE_UNCOPYABLE(FineRectGen)
};

} // namespace cw

#endif // PROJECT_WG_FINE_RECT_GEN_H
