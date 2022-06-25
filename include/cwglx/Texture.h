#ifndef PROJECT_WG_TEXTURE_H
#define PROJECT_WG_TEXTURE_H

#include <vector>
#include <QtGui/qopengl.h>

#include "cwglx/GL.h"
#include "cwglx/Vertex.h"

class QImage;

namespace cw {

class Texture2D {
public:
  Texture2D(const QImage &image, GLFunctions *f);

  [[nodiscard]] GLuint GetTextureId() const noexcept;

  void BeginTexture(GLFunctions *f) const noexcept;
  void ApplyTexture(GLFunctions *f,
                    GLfloat x,
                    GLfloat y) const noexcept;

  void ApplyTexture(GLFunctions *f,
                    const std::array<GLfloat, 2>* points,
                    std::size_t numPoints) const noexcept;

  void ApplyTexture(GLFunctions *f,
                    const std::pair<GLfloat, GLfloat>* points,
                    std::size_t numPoints) const noexcept;

  void DeleteTexture(GLFunctions *f) noexcept;

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
