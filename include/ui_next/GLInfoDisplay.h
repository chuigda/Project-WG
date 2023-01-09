#ifndef PROJECT_WG_UINEXT_GLINFO_H
#define PROJECT_WG_UINEXT_GLINFO_H

#include "ui_next/CloseSignallingWidget.h"

class QLineEdit;
class QPlainTextEdit;
class GLWindow;
class SearchDialog;

class GLInfoDisplay : public CloseSignallingWidget {
public:
  explicit GLInfoDisplay(GLWindow *glWindow);

private slots:
  void LoadGLInfo();

private:
  GLWindow *m_GLWindow;

  QLineEdit *m_Vendor;
  QLineEdit *m_Version;
  QLineEdit *m_Renderer;
  QPlainTextEdit *m_Extensions;
  SearchDialog *m_SearchDialog;
};

#endif // PROJECT_WG_UINEXT_GLINFO_H
