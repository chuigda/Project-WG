#include "ui/GLWidget.h"

#include "glu/FakeGLU.h"
#include "cwglx/Setup.h"

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

GLWidget::~GLWidget() = default;

void GLWidget::initializeGL() {
  cw::SetupPreferredSettings(this);
}

void GLWidget::paintGL() {
  // TODO
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
