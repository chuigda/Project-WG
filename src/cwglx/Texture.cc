#include "cwglx/Texture.h"

#include <QImage>
#include <QOpenGLFunctions_2_0>

namespace cw {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
Texture2D::Texture2D(const QImage &image, QOpenGLFunctions_2_0 *f)
  : m_IsDeleted(false)
{
  Q_ASSERT(image.width() == image.height()
           && "Input image of Texture2D must be square");

  QImage rgbaImage = image.convertToFormat(QImage::Format_RGBA8888);

  f->glGenTextures(1, &m_TextureId);
  f->glBindTexture(GL_TEXTURE_2D, m_TextureId);
  f->glTexParameteri(GL_TEXTURE_2D,
                     GL_TEXTURE_MAG_FILTER,
                     GL_LINEAR);
  f->glTexParameteri(GL_TEXTURE_2D,
                     GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_LINEAR);
  f->glTexParameteri(GL_TEXTURE_2D,
                     GL_GENERATE_MIPMAP,
                     GL_TRUE);
  f->glTexImage2D(GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  rgbaImage.width(),
                  rgbaImage.height(),
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  rgbaImage.bits());
}
#pragma clang diagnostic pop

GLuint Texture2D::GetTextureId() const noexcept {
  Q_ASSERT(!m_IsDeleted && "Texture2D has been deleted");
  return 0;
}

void Texture2D::BeginTexture(QOpenGLFunctions_2_0 *f) const noexcept {
  Q_ASSERT(!m_IsDeleted && "Texture2D has been deleted");
  f->glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

void Texture2D::ApplyTexture(QOpenGLFunctions_2_0 *f,
                             GLfloat x,
                             GLfloat y) const noexcept
{
  f->glTexCoord2f(x, y);
}

void Texture2D::DeleteTexture(QOpenGLFunctions_2_0 *f) noexcept {
  Q_ASSERT(!m_IsDeleted && "Texture2D has been deleted");

  f->glDeleteTextures(1, &m_TextureId);
  m_IsDeleted = true;
}

Texture2D::~Texture2D() noexcept {
  if (!m_IsDeleted) {
    qWarning() << "Texture2D"
               << m_TextureId
               << "has not been deleted before running dtor";
  }
}

} // namespace cw
