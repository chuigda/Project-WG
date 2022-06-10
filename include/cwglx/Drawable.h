#ifndef PROJECT_WG_DRAWABLE_H
#define PROJECT_WG_DRAWABLE_H

#include <utility>
#include <QtGui/qopengl.h>

class QOpenGLFunctions_2_0;

namespace cw {

class Drawable {
public:
  virtual ~Drawable() = 0;

  virtual void Draw(QOpenGLFunctions_2_0 *f) const noexcept = 0;

  virtual void Delete(QOpenGLFunctions_2_0 *f) const noexcept = 0;
};

} // namespace cw

#endif // PROJECT_WG_DRAWABLE_H
