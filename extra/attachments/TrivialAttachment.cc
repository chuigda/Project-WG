#include "wgc0310/api/Attachment.h"

#include <QWidget>
#include "cwglx/GLImpl.h"

class TrivialAttachment final : public wgc0310::WGAPIAttachment {
public:
  TrivialAttachment() : m_Widget(new QWidget(nullptr, Qt::Window)) {
    m_Widget->setWindowTitle("Test");
  }

  ~TrivialAttachment() final {
    delete m_Widget;
  }

  const char *GetName() noexcept final {
    return "Trivial Attachment";
  }

  QWidget *GetControlWidget() noexcept final {
    return m_Widget;
  }

  void NextTick() noexcept final {}

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

wgc0310::WGAPIAttachment * WGAPI LoadAttachment() {
  return new TrivialAttachment();
}
} // extern "C"
