#ifndef PROJECT_WG_TEXTURE_H
#define PROJECT_WG_TEXTURE_H

#include <QtGui/qopengl.h>

class QImage;
class QOpenGLFunctions_2_0;

namespace cw {

class Texture {
public:
  Texture(const QImage &image, QOpenGLFunctions_2_0 *f);

  [[nodiscard]] GLuint GetTextureId() const noexcept;

  void DeleteTexture(QOpenGLFunctions_2_0 *f) noexcept;

  ~Texture() noexcept;

  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;

  Texture(Texture &&) = delete;
  Texture &operator=(Texture &&) = delete;

private:
  GLuint m_TextureId;
  bool m_IsDeleted;
};

} // namespace cw

#endif //PROJECT_WG_TEXTURE_H
