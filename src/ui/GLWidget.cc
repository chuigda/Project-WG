#include "ui/GLWidget.h"

#include <optional>
#include <experimental/array>
#include <QTimerEvent>

#include "glu/FakeGLU.h"
#include "cwglx/Setup.h"
#include "cwglx/Material.h"
#include "cwglx/drawable/Composition.h"
#include "cwglx/drawable/PlainTriangles.h"

GLWidget::GLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    QOpenGLFunctions_2_0(),
    m_Light(nullptr),
    m_Rotation(0.0f)
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

  int timerId = this->startTimer(10);
  if (timerId == 0) {
    qDebug() << "could not start timer correctly";
  }
}

GLWidget::~GLWidget() {
  makeCurrent();

  m_MaterializedTriangles->Delete(this);
  m_PlainTriangles->Delete(this);

  doneCurrent();
}

void GLWidget::timerEvent(QTimerEvent*) {
  this->m_Rotation += 0.5f;
  this->update();
}

void GLWidget::initializeGL() {
  cw::SetupPreferredSettings(this);
  m_Light.reset(new cw::PointLight(GL_LIGHT0,
                                   cw::RGBAColor(32, 32, 32),
                                   cw::RGBAColor(127, 127, 127),
                                   cw::RGBAColor(255, 255, 255),
                                   cw::Vertex(0.0, 1.0, 0.0),
                                   this));
  m_PlainTriangles.reset(new cw::PlainTriangles(
      std::vector { cw::Vertex(0.0, 1.0, 0.0),
                    cw::Vertex(-1.0, -1.0, 0.0),
                    cw::Vertex(1.0, -1.0, 0.0) },
      std::vector { GLuint(0), GLuint(1), GLuint(2),
                    GLuint(0), GLuint(2), GLuint(1) }
  ));
  m_MaterializedTriangles.reset(new cw::MaterializedDrawable(
      cw::GetBrassMaterial(),
      std::vector { const_cast<cw::Drawable const*>(m_PlainTriangles.data()) }
  ));
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  m_Light->Enable(this);
  glTranslatef(0.0f, 0.0f, -5.0f);
  glRotatef(m_Rotation, 0.0f, 1.0f, 0.0f);
  m_MaterializedTriangles->Draw(this);

  glFlush();
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
