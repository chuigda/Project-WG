#include "PaintWidget.h"

#include <QScreen>
#include <QTimer>
#include "cwglx/Setup.h"
#include "cwglx/Material.h"
#include "glu/FakeGLU.h"

PaintWidget::PaintWidget(EntityStatus *entityStatus,
                         cw::Drawable **chosenMesh,
                         cw::Material const**chosenMaterial)
  : m_EntityStatus(entityStatus),
    m_ChosenMesh(chosenMesh),
    m_ChosenMaterial(chosenMaterial),
    m_DevicePixelRatio(1.0)
{
  setMinimumSize(320, 320);

  QTimer *timer = new QTimer(this);
  timer->setInterval(1000 / 30);
  timer->start();
  QObject::connect(timer, &QTimer::timeout, this, [this] { this->update(); });
}

void PaintWidget::RunWithGLContext(const std::function<void(void)> &f) {
  makeCurrent();
  f();
  doneCurrent();
}

void PaintWidget::initializeGL() {
  QOpenGLWidget::initializeGL();
  cw::SetupPreferredSettings(this);

  // Some meshes of WGC-0310 looks weird if you cull the "inner" side
  glDisable(GL_CULL_FACE);

  m_Light = std::make_unique<cw::PointLight>(GL_LIGHT0,
                                             cw::RGBAColor(32, 32, 32),
                                             cw::RGBAColor(187, 187, 187),
                                             cw::RGBAColor(127, 127, 127),
                                             cw::Vertex(-30.0, 15.0, 0.0),
                                             this);

  m_Light2 = std::make_unique<cw::PointLight>(GL_LIGHT1,
                                              cw::RGBAColor(32, 32, 32),
                                              cw::RGBAColor(187, 187, 187),
                                              cw::RGBAColor(127, 127, 127),
                                              cw::Vertex(30.0, 15.0, 0.0),
                                              this);

  m_DevicePixelRatio = this->screen()->devicePixelRatio();

  emit OpenGLInitialized();
}

void PaintWidget::paintGL() {
  GLsizei w = static_cast<GLsizei>(width());
  GLsizei h = static_cast<GLsizei>(height());
  GLdouble wd = static_cast<GLdouble>(w) * m_DevicePixelRatio;
  GLdouble hd = static_cast<GLdouble>(h) * m_DevicePixelRatio;
  w = static_cast<GLsizei>(wd);
  h = static_cast<GLsizei>(hd);

  glViewport(0.0f, 0.0f, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glu::Perspective(this, 45.0, wd / hd, 0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(0.01f, 0.01f, 0.01f);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_Light->Enable(this);
  m_Light2->Enable(this);

  m_EntityStatus->ApplyEntityTransformation(this);

  if (*m_ChosenMaterial) {
    (*m_ChosenMaterial)->Apply(this);
  }

  if (*m_ChosenMesh) {
    (*m_ChosenMesh)->Draw(this);
  }
}

void PaintWidget::resizeGL(int w, int h) {
  Q_UNUSED(w)
  Q_UNUSED(h)
}
