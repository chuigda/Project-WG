#ifndef PROJECT_GL2_TEXTURE_H
#define PROJECT_GL2_TEXTURE_H

#include <vector>
#include <QtGui/qopengl.h>

#include "include/cwglx/GL/GL.h"
#include "util/Derive.h"

class QImage;

namespace cw {

class Texture2D {
public:
  Texture2D(const QImage &image, GLFunctions *f, bool nearest = false);

  [[nodiscard]] GLuint GetTextureId() const noexcept;

  void ActivateTexture(GLFunctions *f,
                       GLenum textureUnit = GL_TEXTURE0,
                       GLint uniform = -1) const noexcept;

  void Delete(GLFunctions *f) noexcept;

  ~Texture2D() noexcept;

  CW_DERIVE_UNCOPYABLE(Texture2D)
  CW_DERIVE_UNMOVABLE(Texture2D)

private:
  GLuint m_TextureId;
  bool m_IsDeleted;
};

} // namespace cw

#endif //PROJECT_GL2_TEXTURE_H
