#include "ui_next/GLInfoDisplay.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QGridLayout>
#include "cwglx/GLInfo.h"
#include "ui_next/GLWindow.h"

GLInfoDisplay::GLInfoDisplay(GLWindow *glWindow)
  : m_GLWindow(glWindow),
    m_Vendor(new QLineEdit()),
    m_Version(new QLineEdit()),
    m_Renderer(new QLineEdit()),
    m_Extensions(new QPlainTextEdit())
{
  setWindowTitle("OpenGL 信息");
  setFixedSize(600, 600);

  QFont monospaceFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  m_Vendor->setFont(monospaceFont);
  m_Version->setFont(monospaceFont);
  m_Renderer->setFont(monospaceFont);
  m_Extensions->setFont(monospaceFont);

  QLabel *vendorLabel = new QLabel("GL_VENDOR");
  QLabel *versionLabel = new QLabel("GL_VERSION");
  QLabel *rendererLabel = new QLabel("GL_RENDERER");
  QLabel *extensionsLabel = new QLabel("GL_EXTENSIONS");
  vendorLabel->setFont(monospaceFont);
  versionLabel->setFont(monospaceFont);
  rendererLabel->setFont(monospaceFont);
  extensionsLabel->setFont(monospaceFont);

  m_Vendor->setReadOnly(true);
  m_Version->setReadOnly(true);
  m_Renderer->setReadOnly(true);
  m_Extensions->setReadOnly(true);
  m_Extensions->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  m_Extensions->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QGridLayout *layout = new QGridLayout();
  this->setLayout(layout);

  layout->addWidget(vendorLabel, 0, 0);
  layout->addWidget(versionLabel, 1, 0);
  layout->addWidget(rendererLabel, 2, 0);
  layout->addWidget(extensionsLabel, 3, 0);

  layout->addWidget(m_Vendor, 0, 1);
  layout->addWidget(m_Version, 1, 1);
  layout->addWidget(m_Renderer, 2, 1);
  layout->addWidget(m_Extensions, 3, 1);

  connect(m_GLWindow, &GLWindow::OpenGLInitialized,
          this, &GLInfoDisplay::LoadGLInfo);
}

void GLInfoDisplay::LoadGLInfo() {
  cw::GLInfo info = cw::GLInfo::AutoDetect(m_GLWindow);

  m_Vendor->setText(info.vendor);
  m_Version->setText(info.version);
  m_Renderer->setText(info.renderer);
  m_Extensions->setPlainText(info.extensions);

  m_Vendor->setCursorPosition(0);
  m_Version->setCursorPosition(0);
  m_Renderer->setCursorPosition(0);
}
