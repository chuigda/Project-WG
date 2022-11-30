#include "ui_next/GLWindow.h"
#include "cwglx/Setup.h"

GLWindow::GLWindow()
  : m_Light(nullptr),
    m_Light2(nullptr),
    m_Mesh(nullptr),
    m_ScreenGlass(nullptr),
    m_Screen(nullptr),
    m_EyeTexture(nullptr),
    m_MouthTexture(nullptr),
    m_MouthOpenTexture(nullptr),
    m_VolumeVBO { 0, 0 },
    m_Timer(nullptr)
{
  QSurfaceFormat format;
  format.setSamples(8);
  format.setDepthBufferSize(16);
  format.setBlueBufferSize(0);
  format.setGreenBufferSize(0);
  format.setRedBufferSize(0);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

  this->setFormat(format);
  this->setBaseSize(QSize(600, 600));

  m_VolumeIndices.reserve(640);
  for (GLuint i = 0; i < 640; i++) {
    m_VolumeIndices.push_back(i);
  }
}

GLWindow::~GLWindow() = default;

void GLWindow::RunWithGLContext(std::function<void(void)> const& f) {
  makeCurrent();
  f();
  doneCurrent();
}

void GLWindow::initializeGL() {
  QOpenGLWindow::initializeGL();

  cw::SetupPreferredSettings(this);
}


void GLWindow::paintGL() {
  GLsizei w = static_cast<GLsizei>(width());
  GLsizei h = static_cast<GLsizei>(height());

  glViewport(0.0f, 0.0f, w, h);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLWindow::resizeGL(int w, int h) {
  Q_UNUSED(w);
  Q_UNUSED(h);
}


