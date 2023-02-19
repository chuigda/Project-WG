#ifndef PROJECT_WG_UINEXT_EXTRA_CONTROL_H
#define PROJECT_WG_UINEXT_EXTRA_CONTROL_H

#include <glm/vec4.hpp>
#include "cwglx/GL/GL.h"
#include "ui_next/CloseSignallingWidget.h"

namespace wgc0310 {
struct ShaderText;
} // namespace wgc0310

struct StatusExtra {
  bool stayOnTop;

  bool customClearColor;
  glm::vec4 clearColor { 0.0f, 0.0f, 0.0f, 1.0f };
};

class GLWindow;
class ShaderEdit;

class ExtraControl final : public CloseSignallingWidget {
  Q_OBJECT

public:
  explicit ExtraControl(GLWindow *glWindow,
                        StatusExtra *statusExtra);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void SetStayOnTop(bool stayOnTop);
#pragma clang diagnostic pop

private:
  GLWindow *m_GLWindow;
  StatusExtra *m_StatusExtra;
};

#endif // PROJECT_WG_UINEXT_EXTRA_CONTROL_H
