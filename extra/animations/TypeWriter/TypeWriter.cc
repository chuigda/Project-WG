#include "cwglx/Texture.h"
#include "cwglx/GLImpl.h"
#include "wgc0310/api/ScreenAnimation.h"
#include "TypeWriterControl.h"

class TypeWriter final : public wgc0310::WGAPIAnimation {
public:
  TypeWriter()
    : m_NeedsUpdate(false),
      m_ControlWidget(new TypeWriterControl(&m_NeedsUpdate)),
      m_Texture(nullptr)
  {}

  ~TypeWriter() final = default;

  const char *GetName() noexcept final {
    return "打字机";
  }

  QWidget *GetControlWidget() noexcept final { return m_ControlWidget.get(); }

  void NextTick() noexcept final {}

  void Rewind() noexcept final {}

  bool Initialize(GLFunctions* f) noexcept final {
    m_Texture = std::make_unique<cw::Texture2D>(m_ControlWidget->capture(), f);
    return true;
  }

  void Draw(GLFunctions* f) noexcept final {
    if (m_NeedsUpdate) {
      m_Texture->UpdateContent(f, m_ControlWidget->capture());
      m_NeedsUpdate = false;
    }

    m_Texture->BeginTexture(f);

    f->glColor3f(1.0f, 1.0f, 1.0f);
    f->glBegin(GL_QUADS);
    {
      f->glTexCoord2f(0.0f, 1.0f);
      f->glVertex2f(-320.0f, 240.0f);

      f->glTexCoord2f(0.0f, 0.0f);
      f->glVertex2f(-320.0f, -240.0f);

      f->glTexCoord2f(1.0f, 0.0f);
      f->glVertex2f(320.0f, -240.0f);

      f->glTexCoord2f(1.0f, 1.0f);
      f->glVertex2f(320.0f, 240.0f);
    }
    f->glEnd();
  }

  void Delete(GLFunctions *f) noexcept final {
    m_Texture->DeleteTexture(f);
  }

private:
  bool m_NeedsUpdate;
  std::unique_ptr<TypeWriterControl> m_ControlWidget;
  std::unique_ptr<cw::Texture2D> m_Texture;
};

extern "C" {
std::uint32_t WGAPI GetWGAPIVersion() {
  return WGAPI_VERSION;
}

wgc0310::WGAPIAnimation *WGAPI LoadAnimation() {
  return new TypeWriter();
}
} // extern "C"
