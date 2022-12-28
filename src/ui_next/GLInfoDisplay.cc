#include "ui_next/GLInfoDisplay.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QMenu>
#include <QTimer>
#include "cwglx/GL/GLImpl.h"
#include "cwglx/GL/GLInfo.h"
#include "ui_next/GLWindow.h"
#include "ui_next/SearchDialog.h"

GLInfoDisplay::GLInfoDisplay(GLWindow *glWindow)
  : m_GLWindow(glWindow),
    m_Vendor(new QLineEdit()),
    m_Version(new QLineEdit()),
    m_Renderer(new QLineEdit()),
    m_Extensions(new QPlainTextEdit()),
    m_SearchDialog(new SearchDialog(m_Extensions, this))
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
  m_Extensions->setContextMenuPolicy(Qt::CustomContextMenu);

  QMenu *menu = new QMenu(m_Extensions);
  menu->addAction("搜索 OpenGL 扩展");
  connect(m_Extensions, &QWidget::customContextMenuRequested,
          this, [menu] { menu->exec(QCursor::pos()); });
  connect(menu, &QMenu::triggered,
          this, [this] { m_SearchDialog->show(); });

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
  cw::GLInfo info = cw::GLInfo::AutoDetect(m_GLWindow->GL);

  m_Vendor->setText(info.vendor);
  m_Version->setText(info.version);
  m_Renderer->setText(info.renderer);
  m_Extensions->setPlainText(info.extensions);

  m_Vendor->setCursorPosition(0);
  m_Version->setCursorPosition(0);
  m_Renderer->setCursorPosition(0);

  if (info.extensions.contains("GL_ARB_timer_query")) {
    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    QGridLayout *layout = static_cast<QGridLayout*>(this->layout());
    QLabel *label = new QLabel("GPU 时间 (ARB)");
    label->setFont(fixedFont);
    layout->addWidget(label, 4, 0);
    QLineEdit *time = new QLineEdit();
    time->setFont(fixedFont);
    time->setReadOnly(true);
    layout->addWidget(time, 4, 1);

    m_GLWindow->EnablePerformanceCounter();

    QTimer *timer = new QTimer(this);
    timer->setInterval(500);
    timer->setTimerType(Qt::VeryCoarseTimer);
    connect(timer, &QTimer::timeout, this, [this, time] {
      this->m_GLWindow->RunWithGLContext([this, time] {
        GLuint64 counter = m_GLWindow->QueryPerformanceCounter();
        double percentage = (static_cast<double>(counter) / 16'666'666.7) * 100.0;
        if (counter != 0) {
          if (counter < 1'000) {
            time->setText(QStringLiteral("%1 ns (%2%)")
                            .arg(counter)
                            .arg(percentage));
          } if (counter < 1'000'000) {
            double us = static_cast<double>(counter) / 1'000.0;
            time->setText(QStringLiteral("%1 μs (%2%)")
                            .arg(static_cast<int>(us))
                            .arg(percentage));
          } else {
            double ms = static_cast<double>(counter) / 1'000'000.0;
            time->setText(QStringLiteral("%1 ms (%2%)")
                            .arg(static_cast<int>(ms))
                            .arg(percentage));
          }
        }
      });
    });
    timer->start();
  }

  if (info.extensions.contains("GL_NVX_gpu_memory_info")) {
    QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    QGridLayout *layout = static_cast<QGridLayout*>(this->layout());
    QLabel *label = new QLabel("显存 (NVX)");
    label->setFont(fixedFont);
    layout->addWidget(label, 5, 0);
    QLineEdit *mem = new QLineEdit();
    mem->setFont(fixedFont);
    mem->setReadOnly(true);
    layout->addWidget(mem, 5, 1);

    QTimer *timer = new QTimer(this);
    timer->setInterval(500);
    timer->setTimerType(Qt::VeryCoarseTimer);
    connect(timer, &QTimer::timeout, this, [this, timer, mem] {
      this->m_GLWindow->RunWithGLContext([this, timer, mem] {
        GLint cur, dedicated;
        m_GLWindow->GL->glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &cur);
        m_GLWindow->GL->glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicated);
        GLenum err = m_GLWindow->GL->glGetError();
        if (err == GL_NO_ERROR) {
          mem->setText(QStringLiteral("%1 MB / %2 MB")
                         .arg((dedicated - cur) / 1024)
                         .arg(dedicated / 1024));
        } else {
          mem->setText("错误");
          qWarning() << "GLInfoDisplay::LoadGLInfo() callback:"
                     << "OpenGL error:"
                     << err;
          timer->stop();
        }
      });
    });
    timer->start();
  }
}
