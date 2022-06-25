#ifndef PROJECT_WG_DRAWABLE_H
#define PROJECT_WG_DRAWABLE_H

#include <utility>
#include <QtGui/qopengl.h>

#include "cwglx/GL.h"

namespace cw {

class Drawable {
public:
  virtual ~Drawable() = 0;

  virtual void Draw(GLFunctions *f) const noexcept = 0;

  virtual void Delete(GLFunctions *f) const noexcept = 0;
};

} // namespace cw

#endif // PROJECT_WG_DRAWABLE_H
