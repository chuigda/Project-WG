#include "cwglx/Drawable.h"

namespace cw {

Drawable::~Drawable() = default;

void Drawable::Delete(GLFunctions *f) const noexcept {
  Q_UNUSED(f);
}

} // namespace cw
