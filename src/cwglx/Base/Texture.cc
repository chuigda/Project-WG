#include "include/cwglx/Base/Texture.h"

#include <QImage>
#include "include/cwglx/GL/GLImpl.h"

namespace cw {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
Texture2D::Texture2D(const QImage &image, GLFunctions *f, bool nearest)
  : m_IsDeleted(false)
{
  QImage rgbaImage = image.convertToFormat(QImage::Format_RGBA8888);

  GLfloat maxAnisotropy = 1.0f;
  f->glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

  f->glGenTextures(1, &m_TextureId);
  f->glBindTexture(GL_TEXTURE_2D, m_TextureId);
  f->glTexParameterf(GL_TEXTURE_2D,
                     GL_TEXTURE_MAX_ANISOTROPY_EXT,
                     maxAnisotropy);
  if (nearest) {
    f->glTexParameteri(GL_TEXTURE_2D,
                       GL_TEXTURE_MAG_FILTER,
                       GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D,
                       GL_TEXTURE_MIN_FILTER,
                       GL_NEAREST);
  } else {
    f->glTexParameteri(GL_TEXTURE_2D,
                       GL_TEXTURE_MAG_FILTER,
                       GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D,
                       GL_TEXTURE_MIN_FILTER,
                       GL_LINEAR_MIPMAP_LINEAR);
  }

  f->glTexImage2D(GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  rgbaImage.width(),
                  rgbaImage.height(),
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  rgbaImage.bits());

  if (!nearest) {
    f->glGenerateMipmap(GL_TEXTURE_2D);
  }

  GLenum error = f->glGetError();
  if (error != GL_NO_ERROR) {
    qCritical() << "Texture2D::Texture2D: failed loading texture:" << error;
    std::abort();
  }
}
#pragma clang diagnostic pop

GLuint Texture2D::GetTextureId() const noexcept {
  Q_ASSERT(!m_IsDeleted && "Texture2D has been deleted");
  return 0;
}

void Texture2D::ActivateTexture(GLFunctions *f,
                                GLenum textureUnit,
                                GLint uniform) const noexcept
{
  Q_ASSERT(!m_IsDeleted && "Texture2D has been deleted");
  f->glActiveTexture(textureUnit);
  f->glBindTexture(GL_TEXTURE_2D, m_TextureId);
  if (uniform >= 0) {
    f->glUniform1i(uniform, static_cast<GLint>(textureUnit) - GL_TEXTURE0);
  }
}

void Texture2D::Delete(GLFunctions *f) noexcept {
  Q_ASSERT(!m_IsDeleted && "Texture2D has been deleted");

  f->glDeleteTextures(1, &m_TextureId);
  m_IsDeleted = true;
}

Texture2D::~Texture2D() noexcept {
  if (!m_IsDeleted) {
    qWarning() << "Texture2D::~Texture2D():"
               << "texture deleted before releasing relevant OpenGL resources";
  }
}

} // namespace cw
