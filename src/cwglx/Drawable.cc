#include "cwglx/Drawable.h"

namespace cw {

Drawable::~Drawable() = default;

void Drawable::Delete(QOpenGLFunctions_2_0 *f) const noexcept {
  Q_UNUSED(f);
}

} // namespace cw
