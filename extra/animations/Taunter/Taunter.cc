#include "wgc0310/api/ScreenAnimation.h"

#include "cwglx/GLImpl.h"
#include "cwglx/Texture.h"

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
    if (m_CurrentTick >= 100) {

    }
  }

  void Rewind() noexcept final {
    m_CurrentTick = 0;
  }

  bool Initialize(GLFunctions*) noexcept final {
    return true;
  }

  void Draw(GLFunctions*) noexcept final {}

  void Delete(GLFunctions*) noexcept final {}

private:
  std::unique_ptr<cw::Texture2D> m_Texture[4];
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
