#include "ui/GLWidget.h"

#include <experimental/array>
#include <QTimerEvent>

#include "glu/FakeGLU.h"
#include "cwglx/Setup.h"
#include "cwglx/Material.h"
#include "cwglx/drawable/Composition.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "cwglx/drawable/TriangleGen.h"
#include "wgc0310/head/Head.h"
#include "wgc0310/head/ScreenGlass.h"
#include "wgc0310/head/Screen.h"

GLWidget::GLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    GLFunctions(),
    m_Light(nullptr),
    m_Light2(nullptr),
    m_Rotation(0.0f),
    m_Arena(),
    m_HeadId(0),
    m_ScreenGlassId(0),
    m_ScreenId(0)
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

  std::unique_ptr<cw::TriangleGen> headGenerator = wgc0310::Head();
  std::unique_ptr<cw::PlainTriangles> headTriangles =
      std::make_unique<cw::PlainTriangles>();
  headTriangles->AddTriangles(headGenerator.get());

  const auto [_, head] = m_Arena.Put(std::move(headTriangles));
  const auto [headId, _2] = m_Arena.Put(
      std::make_unique<cw::MaterializedDrawable>(
          cw::GetBrassMaterial(),
          std::vector { head }
      )
  );
  m_HeadId = headId;

  std::unique_ptr<cw::TriangleGen> glassGenerator = wgc0310::ScreenGlass();
  std::unique_ptr<cw::PlainTriangles> glassTriangles =
      std::make_unique<cw::PlainTriangles>();
  glassTriangles->AddTriangles(glassGenerator.get());

  const auto [_3, glass] = m_Arena.Put(std::move(glassTriangles));
  const auto [glassId, _4] = m_Arena.Put(
      std::make_unique<cw::MaterializedDrawable>(
          cw::GetGlassMaterial(),
          std::vector { glass }
      )
  );
  m_ScreenGlassId = glassId;

  const auto [screenId, _5] = m_Arena.Put(
      std::make_unique<wgc0310::Screen>(this)
  );
  m_ScreenId = screenId;

  const char *version =
      reinterpret_cast<const char*>(glGetString(GL_VERSION));
  const char *vendor =
      reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  const char *renderer =
      reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  const char *extensions =
      reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

  qDebug() << "GL_VERSION:" << version;
  qDebug() << "GL_VENDOR:" << vendor;
  qDebug() << "GL_RENDERER:" << renderer;
  qDebug() << "GL_EXTENSIONS:" << extensions;
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  m_Light->Enable(this);
  m_Light2->Enable(this);

  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -30.0f);
  glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

  m_Arena.Get(m_ScreenId)->Draw(this);

  // m_Arena.Get(m_HeadId)->Draw(this);
  // glTranslatef(0.0f, 0.0f, 1.0f);
  // m_Arena.Get(m_ScreenGlassId)->Draw(this);
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
