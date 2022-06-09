#ifndef PROJECT_WG_TEXTURE_H
#define PROJECT_WG_TEXTURE_H

#include <vector>
#include <QtGui/qopengl.h>

#include "cwglx/Vertex.h"

class QImage;
class QOpenGLFunctions_2_0;

namespace cw {

class Texture2D {
public:
  Texture2D(const QImage &image, QOpenGLFunctions_2_0 *f);

  [[nodiscard]] GLuint GetTextureId() const noexcept;

  void BeginTexture(QOpenGLFunctions_2_0 *f) const noexcept;
  void ApplyTexture(QOpenGLFunctions_2_0 *f,
                    GLfloat x,
                    GLfloat y) const noexcept;

  void ApplyTexture(QOpenGLFunctions_2_0 *f,
                    const std::array<GLfloat, 2>* points,
                    std::size_t numPoints) const noexcept;

  void ApplyTexture(QOpenGLFunctions_2_0 *f,
                    const std::pair<GLfloat, GLfloat>* points,
                    std::size_t numPoints) const noexcept;

  void DeleteTexture(QOpenGLFunctions_2_0 *f) noexcept;

  ~Texture2D() noexcept;

  Texture2D(const Texture2D &) = delete;
  Texture2D &operator=(const Texture2D &) = delete;

  Texture2D(Texture2D &&) = delete;
  Texture2D &operator=(Texture2D &&) = delete;

private:
  GLuint m_TextureId;
  bool m_IsDeleted;
};

} // namespace cw

#endif //PROJECT_WG_TEXTURE_H
