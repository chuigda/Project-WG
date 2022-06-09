#include "ui/GLWidget.h"

#include "glu/FakeGLU.h"
#include "cwglx/Setup.h"
#include "cwglx/drawable/Triangle.h"

#include <experimental/array>

GLWidget::GLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    QOpenGLFunctions_2_0()
{
  QSurfaceFormat format;
  format.setSamples(8);
  format.setDepthBufferSize(16);
  format.setBlueBufferSize(0);
  format.setGreenBufferSize(0);
  format.setRedBufferSize(0);
  this->setFormat(format);

  this->setAttribute(Qt::WA_TranslucentBackground);
  this->setAttribute(Qt::WA_TransparentForMouseEvents);
  this->setAttribute(Qt::WA_AlwaysStackOnTop);
}

GLWidget::~GLWidget() {
  makeCurrent();

  // TODO: Manually delete all loaded textures

  doneCurrent();
}

void GLWidget::initializeGL() {
  cw::SetupPreferredSettings(this);
}

static cw::Triangle triangle( // NOLINT(cert-err58-cpp)
  std::experimental::make_array(
    cw::Vertex { 0.0f, 1.0f, 0.0f },
    cw::Vertex { -1.0f, -1.0f, 0.0f },
    cw::Vertex { 1.0f, -1.0f, 0.0f }
  ),
  std::experimental::make_array(
    cw::RGBAColor { std::uint8_t(255), 0, 0 },
    cw::RGBAColor { 0, std::uint8_t(255), 0 },
    cw::RGBAColor { 0, 0, std::uint8_t(255) }
  )
);

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  glTranslatef(0.0f, 0.0f, -5.0f);
  triangle.Draw(this);
}

void GLWidget::resizeGL(int w, int h) {
  glViewport(0.0f, 0.0f, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glu::Perspective(this,
                   45.0,
                   (static_cast<GLdouble>(w) / static_cast<GLdouble>(h)),
                   0.1,
                   100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
