#include "wgc0310/api/ScreenAnimation.h"

#include <QImage>
#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"
#include "util/Constants.h"

static const std::uint8_t TauntFrameSequence[] = {
  0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 2, 1, 1,
  0, 0, 0, 1, 3, 1,
  0, 0, 1, 3, 1,
  0, 0, 1, 2, 3, 3, 2, 1,
  0, 0, 1, 3, 2, 1,
  0, 1, 1, 3, 3, 2, 1,
  0, 0, 0, 1, 3, 3, 1,
  0, 0, 2, 2, 1,
  0, 2, 3, 1,
  0, 1, 2, 3, 3, 1,
  0, 0, 2, 3, 2,
  0, 0, 1, 1, 3, 3, 3, 1, 1,
  0, 0, 0, 0, 1, 1, 3, 3, 2, 1,
  0, 0, 0, 1, 2, 3, 2, 1,
  0, 0, 0, 0, 1, 2, 3, 2, 1,
  0, 0, 0, 2, 3, 1,
  0, 1, 1, 3, 3, 3, 1, 1,
  0, 0, 0, 1, 3, 3, 1,
  0, 0, 1, 3, 1,
  0, 0, 1, 3, 1,
  0, 0, 1, 3, 3,
  0, 0, 1, 1, 3, 3, 2, 1,
  0, 0, 1, 0
};

class Taunter final : public wgc0310::WGAPIAnimation {
public:
  Taunter() = default;

  ~Taunter() final = default;

  const char *GetName() noexcept final {
    return "电摇嘲讽";
  }

  QWidget *GetControlWidget() noexcept final { return nullptr; }

  void NextTick() noexcept final {
    m_CurrentTick += 1;
    if (m_CurrentTick / 3 >= cw::countof(TauntFrameSequence)) {
      m_CurrentTick = 0;
    }
  }

  void Rewind() noexcept final {
    m_CurrentTick = 0;
  }

  bool Initialize(GLFunctions* f) noexcept final {
    m_TexturePack[0] = std::make_unique<cw::Texture2D>(QImage(":/1.png"), f);
    m_TexturePack[1] = std::make_unique<cw::Texture2D>(QImage(":/2.png"), f);
    m_TexturePack[2] = std::make_unique<cw::Texture2D>(QImage(":/3.png"), f);
    m_TexturePack[3] = std::make_unique<cw::Texture2D>(QImage(":/4.png"), f);
    return true;
  }

  void Draw(GLFunctions* f) noexcept final {
    std::uint8_t frameId = TauntFrameSequence[m_CurrentTick / 3];
    cw::Texture2D *texture = m_TexturePack[frameId].get();

    texture->BeginTexture(f);

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
    for (auto &texture : m_TexturePack) {
      texture->DeleteTexture(f);
    }
  }

private:
  std::array<std::unique_ptr<cw::Texture2D>, 4> m_TexturePack;
  std::size_t m_CurrentTick = 0;
};

extern "C" {
std::uint32_t WGAPI GetWGAPIVersion() {
  return WGAPI_VERSION;
}

wgc0310::WGAPIAnimation * WGAPI LoadAnimation() {
  return new Taunter();
}
} // extern "C"
