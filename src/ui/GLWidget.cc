#include "ui/GLWidget.h"
#include "glu/FakeGLU.h"

GLWidget::GLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    QOpenGLFunctions_1_5()
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
  initializeOpenGLFunctions();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glFrontFace(GL_CCW);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_MULTISAMPLE);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_TEXTURE_2D);
}

void GLWidget::paintGL() {
  // TODO
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0.0f, 0.0f, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glu::perspective(this,
                     45.0,
                     (static_cast<GLdouble>(w)/static_cast<GLdouble>(h)),
                     0.1,
                     100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
