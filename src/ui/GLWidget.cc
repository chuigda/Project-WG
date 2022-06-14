#include "ui/GLWidget.h"

#include <experimental/array>
#include <QTimerEvent>

#include "glu/FakeGLU.h"
#include "cwglx/Setup.h"
#include "cwglx/Material.h"
#include "cwglx/drawable/Composition.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "cwglx/drawable/TriangleGenerator.h"
#include "include/wgc0310/head/Intake.h"

GLWidget::GLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    QOpenGLFunctions_2_0(),
    m_Light(nullptr),
    m_Light2(nullptr),
    m_Rotation(0.0f),
    m_Arena(),
    m_MaterializedId(0)
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

  m_Arena.Delete(this);
  qDebug() << "GLWidget::~GLWidget(): arena released successfully";

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
                                   cw::Vertex(-25.0, 0.0, 0.0),
                                   this));
  m_Light2.reset(new cw::PointLight(GL_LIGHT1,
                                    cw::RGBAColor(32, 32, 32),
                                    cw::RGBAColor(127, 127, 127),
                                    cw::RGBAColor(255, 255, 255),
                                    cw::Vertex(25.0, 0.0, 0.0),
                                    this));

  /*
  cw::Drawable const* sideThermalController =
      wgc0310::SideThermalController(&m_Arena);
  const auto [materializedId, materialized] = m_Arena.Put(
      std::make_unique<cw::MaterializedDrawable>(
          cw::GetBrassMaterial(),
          std::vector { sideThermalController }
      )
  );
  m_MaterializedId = materializedId;
  */
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  m_Light->Enable(this);
  m_Light2->Enable(this);

  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -30.0f);
  glRotatef(-15, 1.0f, 0.0f, 0.0f);
  glRotatef(m_Rotation, 0.0f, 1.0f, 0.0f);

  // m_Arena.Get(m_MaterializedId)->Draw(this);
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
  glScalef(0.01f, 0.01f, 0.01f);
}
