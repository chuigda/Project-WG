#include "ui/GLWidget.h"

#include <experimental/array>
#include <QTimer>
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
#include "ui/ConfigWidget.h"

GLWidget::GLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    GLFunctions(),
    m_Light(nullptr),
    m_Light2(nullptr),
    m_Arena(),
    m_HeadId(0),
    m_ScreenGlassId(0),
    m_ScreenId(0),
    m_Timer(new QTimer(this))
{
  QSurfaceFormat format;
  format.setSamples(8);
  format.setDepthBufferSize(16);
  format.setBlueBufferSize(0);
  format.setGreenBufferSize(0);
  format.setRedBufferSize(0);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

  this->setFormat(format);

  this->setAttribute(Qt::WA_TranslucentBackground);
  this->setAttribute(Qt::WA_TransparentForMouseEvents);
  this->setAttribute(Qt::WA_AlwaysStackOnTop);

  m_ConfigWidget = new ConfigWidget(&m_CameraEntityStatus,
                                    &m_ScreenStatus,
                                    this);
  connect(m_ConfigWidget, &ConfigWidget::SetRenderSettings,
          this, &GLWidget::OnRenderSettingsSet);

  setBaseSize(1024, 768);
  resize(1024,768);

  connect(this, &GLWidget::StaticScreensLoaded,
          m_ConfigWidget, &ConfigWidget::OnStaticScreensLoaded);
  connect(this, &GLWidget::AnimationsLoaded,
          m_ConfigWidget, &ConfigWidget::OnAnimationsLoaded);

  m_Timer->setInterval(1000 / 90);
  connect(m_Timer, &QTimer::timeout, this, &GLWidget::RequestNextFrame);
  m_Timer->start();

  LoadAnimations();
}

GLWidget::~GLWidget() {
  makeCurrent();

  m_Arena.Delete(this);
  qDebug() << "GLWidget::~GLWidget(): arena released successfully";

  for (const std::unique_ptr<cw::Texture2D>& item : m_StaticScreens) {
    item->DeleteTexture(this);
  }
  qDebug() << "GLWidget::~GLWidget():"
           << "static screen textures released successfully";

  for (const std::unique_ptr<Animation>& item : m_Animations) {
    item->Delete(this);
  }
  qDebug() << "GLWidget::~GLWidget():"
           << "animations released successfully";

  doneCurrent();
}

void GLWidget::initializeGL() {
  cw::SetupPreferredSettings(this);
  m_Light.reset(new cw::PointLight(GL_LIGHT0,
                                   cw::RGBAColor(32, 32, 32),
                                   cw::RGBAColor(127, 127, 127),
                                   cw::RGBAColor(255, 255, 255),
                                   cw::Vertex(-25.0, 0.0, 0),
                                   this));
  m_Light2.reset(new cw::PointLight(GL_LIGHT1,
                                    cw::RGBAColor(32, 32, 32),
                                    cw::RGBAColor(127, 127, 127),
                                    cw::RGBAColor(255, 255, 255),
                                    cw::Vertex(25.0, 0.0, 0),
                                    this));

  std::unique_ptr<cw::TriangleGen> headGenerator = wgc0310::Head();
  std::unique_ptr<cw::PlainTriangles> headTriangles =
      std::make_unique<cw::PlainTriangles>();
  headTriangles->AddTriangles(headGenerator.get());
  headTriangles->PreInitialize(this);

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
  glassTriangles->PreInitialize(this);

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

  cw::GLInfo glInfo = cw::GLInfo::AutoDetect(this);
  m_ConfigWidget->FillGLInfo(glInfo);

  LoadAndInitScreens();
  InitAnimations();
}

void GLWidget::paintGL() {
  // preparation stage
  wgc0310::Screen const* screen =
      static_cast<wgc0310::Screen const*>(m_Arena.Get(m_ScreenId));
  screen->BeginScreenContext(this);
  m_ScreenStatus.DrawOnScreen(this);
  screen->DoneScreenContext(this);

  // switch back to default framebuffer. not using `0` since
  // Qt doesn't use it as the default one.
  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

  // painting stage
  GLsizei w = static_cast<GLsizei>(width());
  GLsizei h = static_cast<GLsizei>(height());
  GLdouble wd = static_cast<GLdouble>(w);
  GLdouble hd = static_cast<GLdouble>(h);

  glViewport(0.0f, 0.0f, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glu::Perspective(this, 45.0, wd / hd, 0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glScalef(0.01f, 0.01f, 0.01f);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_CameraEntityStatus.ApplyCameraTransformation(this);
  m_Light->Enable(this);
  m_Light2->Enable(this);
  m_CameraEntityStatus.ApplyEntityTransformation(this);

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  m_Arena.Get(m_HeadId)->Draw(this);

  screen->Draw(this);

  glTranslatef(0.0f, 0.0f, 0.5f);
  glPushAttrib(GL_CURRENT_COLOR);
  glColor4f(0.05f, 0.075f, 0.1f, 0.1f);
  m_Arena.Get(m_ScreenGlassId)->Draw(this);
  glPopAttrib();
}

void GLWidget::resizeGL(int w, int h) {
  Q_UNUSED(w)
  Q_UNUSED(h)
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  Q_UNUSED(event)

  m_ConfigWidget->show();
}

void GLWidget::OnRenderSettingsSet(GLenum cullFaceMode, int targetFPS) {
  makeCurrent();
  if (cullFaceMode == GL_NONE) {
    glDisable(GL_CULL_FACE);
  } else {
    glEnable(GL_CULL_FACE);
    glFrontFace(cullFaceMode);
  }
  doneCurrent();

  m_Timer->setInterval(1000 / targetFPS);
}
