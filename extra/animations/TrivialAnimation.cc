#include "wgc0310/api/ScreenAnimation.h"

#include <QWidget>
#include "cwglx/GLImpl.h"

class TrivialAnimation final : public wgc0310::WGAPIAnimation {
public:
  TrivialAnimation() : m_Widget(new QWidget(nullptr, Qt::Window)) {
    m_Widget->setWindowTitle("Test");
  }

  ~TrivialAnimation() final {
    delete m_Widget;
  }

  const char *GetName() noexcept final {
    return "Trivial Animation";
  }

  QWidget *GetControlWidget() noexcept final {
    return m_Widget;
  }

  void NextTick() noexcept final {}

  void Rewind() noexcept final {}

  bool Initialize(GLFunctions*) noexcept final {
    return true;
  }

  void Draw(GLFunctions*) noexcept final {}

  void Delete(GLFunctions*) noexcept final {}

private:
  QWidget *m_Widget;
};

extern "C" {
std::uint32_t WGAPI GetWGAPIVersion() {
  return WGAPI_VERSION;
}

wgc0310::WGAPIAnimation * WGAPI LoadAnimation() {
  return new TrivialAnimation();
}
} // extern "C"
