#include "ui/GLWidget.h"

#include <experimental/array>
#include <QTimer>
#include <QTimerEvent>
#include <QElapsedTimer>

#include "glu/FakeGLU.h"
#include "cwglx/Setup.h"
#include "cwglx/Material.h"
#include "cwglx/MeshLoader.h"
#include "cwglx/drawable/Composition.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "cwglx/drawable/TriangleGen.h"
#include "include/wgc0310/ScreenGlass.h"
#include "include/wgc0310/Screen.h"
#include "ui/ConfigWidget.h"

GLWidget::GLWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    GLFunctions(),
    m_Light(nullptr),
    m_Light2(nullptr),
    m_Arena(),
    m_MonitorId(0),
    m_MonitorIntakeId(0),
    m_RadarId(0),
    m_ChestBoxId(0),
    m_ChestPlateId(0),
    m_PowerId(0),
    m_PowerPinId(0),
    m_BerId(0),
    m_BerShellId(0),
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
                                   cw::RGBAColor(187, 187, 187),
                                   cw::RGBAColor(127, 127, 127),
                                   cw::Vertex(-30.0, 15.0, 10.0),
                                   this));
  m_Light2.reset(new cw::PointLight(GL_LIGHT1,
                                    cw::RGBAColor(32, 32, 32),
                                    cw::RGBAColor(187, 187, 187),
                                    cw::RGBAColor(127, 127, 127),
                                    cw::Vertex(30.0, 15.0, 10.0),
                                    this));

  const auto plastic = cw::GetPlasticMaterial();
  const auto chrome = cw::GetChromeMaterial();
  const auto brass = cw::GetBrassMaterial();
  const auto blackPlastic = cw::GetBlackPlasticMaterial();
  const auto glass = cw::GetGlassMaterial();

  #define LOAD_MESH(PATH, MTL, TGT) \
    { \
      std::unique_ptr<cw::PlainTriangles> triangles = cw::LoadMesh(PATH); \
      triangles->PreInitialize(this); \
      const auto [_, meshPtr] = m_Arena.Put(std::move(triangles)); \
      const auto [meshId, _2] = \
        m_Arena.Put( \
          std::make_unique<cw::MaterializedDrawable>( \
            MTL, std::vector { meshPtr } \
          ) \
        ); \
      TGT = meshId; \
    }

  LOAD_MESH("./resc/model/monitor.mesh", plastic, m_MonitorId)
  LOAD_MESH("./resc/model/monitor-intake.mesh", chrome, m_MonitorIntakeId)
  LOAD_MESH("./resc/model/radar.mesh", brass, m_RadarId)

  LOAD_MESH("./resc/model/chest-box.mesh", plastic, m_ChestBoxId)
  LOAD_MESH("./resc/model/chest-plate.mesh", chrome, m_ChestPlateId)
  LOAD_MESH("./resc/model/power.mesh", blackPlastic, m_PowerId)
  LOAD_MESH("./resc/model/power-pin.mesh", brass, m_PowerPinId)
  LOAD_MESH("./resc/model/ber-shell.mesh", glass, m_BerShellId)

  #undef LOAD_MESH

  // the 'ber', does not use material
  {
    std::unique_ptr<cw::PlainTriangles> berTriangles =
      cw::LoadMesh("./resc/model/ber.mesh");
    berTriangles->PreInitialize(this);
    const auto [berId, _] = m_Arena.Put(std::move(berTriangles));
    m_BerId = berId;
  }

  // screen
  {
    const auto [screenId, _5] =
      m_Arena.Put(std::make_unique<wgc0310::Screen>(this));
    m_ScreenId = screenId;
  }

  // screen glass
  {
    std::unique_ptr<cw::TriangleGen> glassGenerator = wgc0310::ScreenGlass();
    std::unique_ptr<cw::PlainTriangles> glassTriangles =
      std::make_unique<cw::PlainTriangles>();
    glassTriangles->AddTriangles(glassGenerator.get());
    glassTriangles->PreInitialize(this);

    const auto [_3, glassMeshPtr] = m_Arena.Put(std::move(glassTriangles));
    const auto [glassId, _4] = m_Arena.Put(
      std::make_unique<cw::MaterializedDrawable>(
        glass,
        std::vector { glassMeshPtr }
      )
    );
    m_ScreenGlassId = glassId;
  }

  // detect OpenGL information
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

  glColor4f(0, 0.5f, 1.0f, 1.0f);
  glDisable(GL_LIGHTING);
  m_Arena.Get(m_BerId)->Draw(this);
  glEnable(GL_LIGHTING);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  glPushMatrix();
  {
    m_Arena.Get(m_ChestBoxId)->Draw(this);
    m_Arena.Get(m_ChestPlateId)->Draw(this);
    m_Arena.Get(m_PowerId)->Draw(this);
    m_Arena.Get(m_PowerPinId)->Draw(this);

    glTranslatef(0.0f, 13.5f, 0.0f);
    m_Arena.Get(m_MonitorId)->Draw(this);
    m_Arena.Get(m_MonitorIntakeId)->Draw(this);

    glTranslatef(0.0f, 9.25f, 0.0f);
    glPushMatrix();
    glRotatef(m_RadarRotation, 1.0f, 0.0f, 0.0f);
    m_Arena.Get(m_RadarId)->Draw(this);
    glPopMatrix();

    glTranslatef(0.0f, 0.0f, 4.5f);
    screen->Draw(this);

    glTranslatef(0.0f, 0.0f, 0.5f);
    glColor4f(0.05f, 0.075f, 0.1f, 0.1f);
    m_Arena.Get(m_ScreenGlassId)->Draw(this);
  }
  glPopMatrix();
  m_Arena.Get(m_BerShellId)->Draw(this);
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
