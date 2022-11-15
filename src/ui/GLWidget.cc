#include "ui/GLWidget.h"

#include <experimental/array>
#include <QTimer>
#include <QTimerEvent>
#include <QElapsedTimer>

#include "glu/FakeGLU.h"
#include "cwglx/Setup.h"
#include "cwglx/Material.h"
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
    m_Mesh(nullptr),
    m_ScreenGlass(nullptr),
    m_Screen(nullptr),
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
                                    &m_BodyStatus,
                                    &m_ScreenStatus,
                                    this);
  connect(m_ConfigWidget, &ConfigWidget::SetRenderSettings,
          this, &GLWidget::OnRenderSettingsSet);

  setBaseSize(1024, 768);
  resize(1024, 768);

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


  m_Mesh = std::make_unique<wgc0310::WGCMeshCollection>(this, m_Arena);

  // screen
  {
    const auto [_, screen] =
      m_Arena.Put(std::make_unique<wgc0310::Screen>(this));
    m_Screen = static_cast<wgc0310::Screen const*>(screen);
  }

  // screen glass
  {
    std::unique_ptr<cw::TriangleGen> glassGenerator = wgc0310::ScreenGlass();
    std::unique_ptr<cw::PlainTriangles> glassTriangles =
      std::make_unique<cw::PlainTriangles>();
    glassTriangles->AddTriangles(glassGenerator.get());
    glassTriangles->PreInitialize(this);

    const auto [_, glassMeshPtr] = m_Arena.Put(std::move(glassTriangles));
    const auto [_2, mtlMeshPtr] = m_Arena.Put(
      std::make_unique<cw::MaterializedDrawable>(
        cw::GetGlassMaterial(),
        std::vector { glassMeshPtr }
      )
    );
    m_ScreenGlass = mtlMeshPtr;
  }

  // detect OpenGL information
  cw::GLInfo glInfo = cw::GLInfo::AutoDetect(this);
  m_ConfigWidget->FillGLInfo(glInfo);

  LoadAndInitScreens();
  InitAnimations();
}

void GLWidget::paintGL() {
  // preparation stage
  m_Screen->BeginScreenContext(this);
  m_ScreenStatus.DrawOnScreen(this);
  m_Screen->DoneScreenContext(this);

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

  cw::RGBAColorF berColor = m_BodyStatus.GetTimerColor();
  berColor.Apply(this);
  glDisable(GL_LIGHTING);
  m_Mesh->ber->Draw(this);
  glEnable(GL_LIGHTING);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  glPushMatrix();
  {
    m_Mesh->chestBox->Draw(this);
    m_Mesh->chestPlate->Draw(this);
    m_Mesh->power->Draw(this);
    m_Mesh->powerPin->Draw(this);

    m_Mesh->abdomen->Draw(this);
    m_Mesh->waist->Draw(this);

    glPushMatrix();
    {
      glTranslatef(14.5f, 7.5f, 0.0f);
      m_Mesh->shoulder->Draw(this);
      m_Mesh->shoulderPlate->Draw(this);

      glTranslatef(4.75, 0.0f, 0.0f);
      glRotatef(-75.0f, 0.0f, 0.0f, 1.0f);
      m_Mesh->wheelSmall->Draw(this);
      m_Mesh->bigArm->Draw(this);
      m_Mesh->bigArmCover->Draw(this);

      glTranslatef(20.0, 0.0f, 0.0f);
      m_Mesh->bigArmConnector->Draw(this);

      glTranslatef(4.5, 0.0f, 0.0f);
      glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
      m_Mesh->wheelSmall->Draw(this);
      m_Mesh->smallArm->Draw(this);
      m_Mesh->smallArmCover->Draw(this);

      glTranslatef(25.0f, 0.0f, 0.0f);
      glRotatef(-7.5f, 0.0f, 0.0f, 1.0f);
      m_Mesh->claw->Draw(this);
      m_Mesh->clawCover->Draw(this);
    }
    glPopMatrix();

    glPushMatrix();
    glScalef(-1.0f, 1.0f, -1.0f);
    {
      glTranslatef(14.5f, 7.5f, 0.0f);
      m_Mesh->shoulder->Draw(this);
      m_Mesh->shoulderPlate->Draw(this);

      glTranslatef(4.75, 0.0f, 0.0f);
      glRotatef(-75.0f, 0.0f, 0.0f, 1.0f);
      m_Mesh->wheelSmall->Draw(this);
      m_Mesh->bigArm->Draw(this);
      m_Mesh->bigArmCover->Draw(this);

      glTranslatef(20.0, 0.0f, 0.0f);
      m_Mesh->bigArmConnector->Draw(this);

      glTranslatef(4.5, 0.0f, 0.0f);
      glRotatef(-15.0f, 0.0f, 0.0f, 1.0f);
      m_Mesh->wheelSmall->Draw(this);
      m_Mesh->smallArm->Draw(this);
      m_Mesh->smallArmCover->Draw(this);

      glTranslatef(25.0f, 0.0f, 0.0f);
      glRotatef(-7.5f, 0.0f, 0.0f, 1.0f);
      m_Mesh->claw->Draw(this);
      m_Mesh->clawCover->Draw(this);
    }
    glPopMatrix();

    glTranslatef(0.0f, 12.875f, 0.0f);
    m_Mesh->wheel->Draw(this);

    glTranslatef(0.0f, 0.375f, 0.0f);
    m_Mesh->monitor->Draw(this);
    m_Mesh->monitorIntake->Draw(this);

    glTranslatef(0.0f, 9.25f + 1.875f, 4.5f);
    m_Screen->Draw(this);

    glTranslatef(0.0f, 0.0f, 0.5f);
    glColor4f(0.05f, 0.075f, 0.1f, 0.1f);
    m_ScreenGlass->Draw(this);
  }
  glPopMatrix();
  m_Mesh->berShell->Draw(this);
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
