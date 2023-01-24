#ifndef PROJECT_WG_SHADER_EDIT_H
#define PROJECT_WG_SHADER_EDIT_H

#include <QWidget>
#include "wgc0310/Shader.h"
#include "ui_next/CloseSignallingWidget.h"

namespace wgc0310 {
struct ShaderText;
} // namespace wgc0310

class GLWindow;

class ShaderEdit : public CloseSignallingWidget {
  Q_OBJECT

public:
  explicit ShaderEdit(GLWindow *glWindow);

private:
  wgc0310::ShaderText m_ShaderText;
  GLWindow *m_GLWindow;

  int m_ShaderPrevIndex;
  int m_ShaderSubPrevIndex;
};

#endif // PROJECT_WG_SHADER_EDIT_H
