#include "ui_next/GLWindow.h"

#include <QTimer>
#include <QCloseEvent>
#include "cwglx/Setup.h"
#include "cwglx/drawable/TriangleGen.h"
#include "wgc0310/ScreenGlass.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "glu/FakeGLU.h"

GLWindow::GLWindow(EntityStatus const* entityStatus,
                   wgc0310::HeadStatus const* headStatus,
                   wgc0310::BodyStatus const* bodyStatus,
                   wgc0310::ScreenAnimationStatus const *screenAnimationStatus,
                   cw::CircularBuffer<qreal, 160> *volumeLevels,
                   bool *volumeLevelsUpdated,
                   wgc0310::ScreenDisplayMode const *screenDisplayMode)
  : QOpenGLWidget(nullptr, Qt::Window),
    // Input status
    m_EntityStatus(entityStatus),
    m_HeadStatus(headStatus),
    m_BodyStatus(bodyStatus),
    m_ScreenAnimationStatus(screenAnimationStatus),
    m_VolumeLevels(volumeLevels),
    m_VolumeLevelsUpdated(volumeLevelsUpdated),
    m_ScreenDisplayMode(screenDisplayMode),
    // Internal states and OpenGL resources
    m_Light(nullptr),
    m_Light2(nullptr),
    m_ScreenGlass(nullptr),
    m_Screen(nullptr),
    m_EyeTexture(nullptr),
    m_MouthTexture(nullptr),
    m_MouthOpenTexture(nullptr),
    m_VolumeIndices {},
    m_VolumeVBO { 0, 0 }
{
  setWindowTitle("Project-WG - 绘图输出窗口");
  setWindowFlags(Qt::CustomizeWindowHint
                 | Qt::WindowTitleHint
                 | Qt::WindowMaximizeButtonHint);

  QSurfaceFormat format;
  format.setSamples(8);
  format.setDepthBufferSize(16);
  format.setBlueBufferSize(0);
  format.setGreenBufferSize(0);
  format.setRedBufferSize(0);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

  this->setAttribute(Qt::WA_TranslucentBackground);

  this->setFormat(format);
  this->resize(600, 600);

  for (GLuint i = 0; i < 640; i++) {
    m_VolumeIndices[i] = i;
  }
}

GLWindow::~GLWindow() {
  RunWithGLContext([this] {
    m_Arena.Delete(this);

    m_EyeTexture->DeleteTexture(this);
    m_MouthTexture->DeleteTexture(this);
    m_MouthOpenTexture->DeleteTexture(this);

    glDeleteBuffers(2, m_VolumeVBO.data());
  });
}

void GLWindow::RunWithGLContext(std::function<void(void)> const& f) {
  makeCurrent();
  f();
  doneCurrent();
}

void GLWindow::initializeGL() {
  QOpenGLWidget::initializeGL();
  cw::SetupPreferredSettings(this);

  m_Light = std::make_unique<cw::PointLight>(GL_LIGHT0,
                                             cw::RGBAColor(32, 32, 32),
                                             cw::RGBAColor(187, 187, 187),
                                             cw::RGBAColor(127, 127, 127),
                                             cw::Vertex(-30.0, 15.0, 10.0),
                                             this);
  m_Light2 = std::make_unique<cw::PointLight>(GL_LIGHT1,
                                              cw::RGBAColor(32, 32, 32),
                                              cw::RGBAColor(187, 187, 187),
                                              cw::RGBAColor(127, 127, 127),
                                              cw::Vertex(30.0, 15.0, 10.0),
                                              this);
  m_Mesh.Load(this, m_Arena);

  // screen
  {
    const auto screen = m_Arena.Put(std::make_unique<wgc0310::Screen>(this));
    m_Screen = static_cast<wgc0310::Screen const*>(screen);
  }

  // screen glass
  {
    std::unique_ptr<cw::TriangleGen> glassGenerator = wgc0310::ScreenGlass();
    std::unique_ptr<cw::PlainTriangles> glassTriangles =
      std::make_unique<cw::PlainTriangles>();
    glassTriangles->AddTriangles(glassGenerator.get());
    glassTriangles->PreInitialize(this);

    const auto glassMeshPtr = m_Arena.Put(std::move(glassTriangles));
    const auto mtlMeshPtr = m_Arena.Put(
      std::make_unique<cw::MaterializedDrawable>(
        cw::GetGlassMaterial(),
        std::vector { glassMeshPtr }
      )
    );
    m_ScreenGlass = mtlMeshPtr;
  }

  {
    QImage eye9Image(":/eye.9.bmp");
    QImage closeMouthImage(":/half-face.bmp");
    QImage openMouthImage(":/half-face-mouth-open.bmp");
    Q_ASSERT(!eye9Image.isNull());
    Q_ASSERT(!closeMouthImage.isNull());
    Q_ASSERT(!openMouthImage.isNull());

    m_EyeTexture = std::make_unique<cw::Texture2D>(eye9Image, this);
    m_MouthTexture = std::make_unique<cw::Texture2D>(closeMouthImage, this);
    m_MouthOpenTexture = std::make_unique<cw::Texture2D>(openMouthImage, this);

    glGenBuffers(2, m_VolumeVBO.data());
    glBindBuffer(GL_ARRAY_BUFFER, m_VolumeVBO[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(m_VolumeVertices.size() * sizeof(cw::Vertex2DF)),
                 m_VolumeVertices.data(),
                 GL_STREAM_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VolumeVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizei>(m_VolumeIndices.size() * sizeof(GLuint)),
                 m_VolumeIndices.data(),
                 GL_STATIC_DRAW);
  }

  {
    for (GLuint i = 0; i < 160; i++) {
      float x = static_cast<float>(i) * 4.0f - 320.0f;
      m_VolumeVertices[i * 4] = cw::Vertex2DF(x, 0.1f);
      m_VolumeVertices[i * 4 + 1] = cw::Vertex2DF(x, -0.1f);
      m_VolumeVertices[i * 4 + 2] = cw::Vertex2DF(x + 4.0f, -0.1f);
      m_VolumeVertices[i * 4 + 3] = cw::Vertex2DF(x + 4.0f, 0.1f);
    }
  }

  emit OpenGLInitialized();
}

void GLWindow::paintGL() {
  // prepare screen content
  m_Screen->BeginScreenContext(this);
  DrawScreenContent();
  m_Screen->DoneScreenContext(this);

  // switch back to default frame buffer.
  // not using `0` since Qt doesn't use it as the default one.
  glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

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

  m_Light->Enable(this);
  m_Light2->Enable(this);
  m_EntityStatus->ApplyEntityTransformation(this);

  glTranslatef(0.0f, -12.0f, 0.0f);

  for (std::size_t i = 0; i < 10; i++) {
    m_Mesh.waist->Draw(this);
    glTranslatef(0.0f, 1.0f, 0.0f);
    glRotatef(m_HeadStatus->rotationZ / 10.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(m_HeadStatus->rotationX / 10.0f, 1.0f, 0.0f, 0.0f);
    m_Mesh.abdomen->Draw(this);
    glTranslatef(0.0f, 1.0f, 0.0f);
  }

  glTranslatef(0.0f, 6.0f, 0.0f);

  cw::RGBAColorF berColor = m_BodyStatus->GetTimerColor();
  berColor.Apply(this);
  glDisable(GL_LIGHTING);
  m_Mesh.colorTimer->Draw(this);
  glEnable(GL_LIGHTING);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  glPushMatrix();
  {
    m_Mesh.chestBox->Draw(this);
    m_Mesh.chestPlate->Draw(this);
    m_Mesh.power->Draw(this);
    m_Mesh.powerPin->Draw(this);

    glPushMatrix();
    DrawArm(m_BodyStatus->rightArmStatus, 1.0f);
    glPopMatrix();

    glPushMatrix();
    glScalef(-1.0f, 1.0f, -1.0f);
    DrawArm(m_BodyStatus->leftArmStatus, -1.0f);
    glPopMatrix();

    glTranslatef(0.0f, 12.875f, 0.0f);
    glRotatef(m_HeadStatus->rotationY / 2, 0.0f, 1.0f, 0.0f);
    m_Mesh.wheel->Draw(this);
    glRotatef(m_HeadStatus->rotationY / 2, 0.0f, 1.0f, 0.0f);

    glTranslatef(0.0f, 0.375f, 0.0f);
    m_Mesh.monitor->Draw(this);
    m_Mesh.monitorIntake->Draw(this);

    glTranslatef(0.0f, 9.25f + 1.875f, 4.5f);
    m_Screen->Draw(this);

    glTranslatef(0.0f, 0.0f, 0.5f);
    glColor4f(0.05f, 0.075f, 0.1f, 0.1f);
    m_ScreenGlass->Draw(this);
  }
  glPopMatrix();
  m_Mesh.colorTimerShell->Draw(this);
}

void GLWindow::resizeGL(int w, int h) {
  Q_UNUSED(w)
  Q_UNUSED(h)
}

static void DrawEye(GLFunctions *f,
                    float top,
                    float bottom,
                    float left,
                    float right)
{
  f->glBegin(GL_QUADS);
  {
    f->glTexCoord2f(0.0f, 0.0f);
    f->glVertex2f(left, top);

    f->glTexCoord2f(0.0f, 0.25f);
    f->glVertex2f(left, top - 7.0f);

    f->glTexCoord2f(1.0f, 0.25f);
    f->glVertex2f(right, top - 7.0f);

    f->glTexCoord2f(1.0f, 0.0f);
    f->glVertex2f(right, top);
  }

  {
    f->glTexCoord2f(0.0f, 0.25f);
    f->glVertex2f(left, top - 7.0f);

    f->glTexCoord2f(0.0f, 0.75f);
    f->glVertex2f(left, bottom + 7.0f);

    f->glTexCoord2f(1.0f, 0.75f);
    f->glVertex2f(right, bottom + 7.0f);

    f->glTexCoord2f(1.0f, 0.25f);
    f->glVertex2f(right, top - 7.0f);
  }

  {
    f->glTexCoord2f(0.0f, 0.75f);
    f->glVertex2f(left, bottom + 7.0f);

    f->glTexCoord2f(0.0f, 1.0f);
    f->glVertex2f(left, bottom);

    f->glTexCoord2f(1.0f, 1.0f);
    f->glVertex2f(right, bottom);

    f->glTexCoord2f(1.0f, 0.75f);
    f->glVertex2f(right, bottom + 7.0f);
  }
  f->glEnd();
}

void GLWindow::DrawScreenContent() {
  if (m_ScreenAnimationStatus->HasThingToDraw()) {
    m_ScreenAnimationStatus->DrawOnScreen(this);
  } else {
    glPushMatrix();
    glScalef(1.0f, -1.0f, 1.0f);
    glFrontFace(GL_CW);
    if (*m_ScreenDisplayMode == wgc0310::ScreenDisplayMode::SoundWave) {
      glPushMatrix();
      glColor4f(0.5f, 1.0f, 1.0f, 1.0f);
      glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
      {
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, m_VolumeVBO[0]);
        glVertexPointer(2, GL_FLOAT, 0, nullptr);
        if (*m_VolumeLevelsUpdated) {
          *m_VolumeLevelsUpdated = false;
          for (size_t i = 0; i < 160; i++) {
            m_VolumeVertices[i * 4].SetY(static_cast<GLfloat>(m_VolumeLevels->Get(i)) * 200.0f);
            m_VolumeVertices[i * 4 + 1].SetY(static_cast<GLfloat>(m_VolumeLevels->Get(i)) * -200.0f);
            m_VolumeVertices[i * 4 + 2].SetY(static_cast<GLfloat>(m_VolumeLevels->Get(i)) * -200.0f);
            m_VolumeVertices[i * 4 + 3].SetY(static_cast<GLfloat>(m_VolumeLevels->Get(i)) * 200.0f);
          }

          glBufferData(GL_ARRAY_BUFFER,
                       static_cast<GLsizei>(m_VolumeVertices.size() * sizeof(cw::Vertex2DF)),
                       m_VolumeVertices.data(),
                       GL_STREAM_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VolumeVBO[1]);
        glDrawElements(GL_QUADS,
                       static_cast<GLsizei>(m_VolumeIndices.size()),
                       GL_UNSIGNED_INT,
                       nullptr);
      }
      glPopClientAttrib();
      glPopMatrix();
    } else {
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      if (m_HeadStatus->mouthStatus == wgc0310::HeadStatus::MouthStatus::Close) {
        m_MouthTexture->BeginTexture(this);
      } else {
        m_MouthOpenTexture->BeginTexture(this);
      }

      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(-320.0f, 0.0f);

      glTexCoord2f(0.0f, 1.0f);
      glVertex2f(-320.0f, -240.0f);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(320.0f, -240.0f);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(320.0f, 0.0f);
      glEnd();

      float leftEyeTop = 42.0f + 4.0f + 48.0f * m_HeadStatus->leftEye;
      float leftEyeBottom = 42.0f - 4.0f - 48.0f * m_HeadStatus->leftEye;
      float rightEyeTop = 42.0f + 4.0f + 48.0f * m_HeadStatus->rightEye;
      float rightEyeBottom = 42.0f - 4.0f - 48.0f * m_HeadStatus->rightEye;

      glTranslatef(0.0f, 0.0f, 0.1f);

      glPushMatrix();
      glScalef(1.0f / 0.75f, 1.0f / 0.75f, 1.0f);
      m_EyeTexture->BeginTexture(this);

      DrawEye(this, leftEyeTop, leftEyeBottom, -118.0f, -90.0f);
      DrawEye(this, rightEyeTop, rightEyeBottom, 90.0f, 118.0f);

      glPopMatrix();
    }
    glPopMatrix();
  }
}

void GLWindow::DrawArm(const wgc0310::ArmStatus &armStatus, GLfloat coeff) {
  glTranslatef(14.5f, 7.5f, 0.0f);
  glRotatef(coeff * armStatus.rotation[0], 1.0f, 0.0f, 0.0f);
  m_Mesh.shoulder->Draw(this);
  m_Mesh.shoulderPlate->Draw(this);

  glTranslatef(4.75, 0.0f, 0.0f);
  glRotatef(armStatus.rotation[1] / 2.0f, 0.0f, 0.0f, 1.0f);
  m_Mesh.wheelSmall->Draw(this);
  glRotatef(armStatus.rotation[1] / 2.0f, 0.0f, 0.0f, 1.0f);
  m_Mesh.bigArm->Draw(this);
  m_Mesh.bigArmCover->Draw(this);

  glTranslatef(20.0, 0.0f, 0.0f);
  glRotatef(-1.0f * armStatus.rotation[2], 1.0f, 0.0f, 0.0f);
  m_Mesh.bigArmConnector->Draw(this);

  glTranslatef(4.5, 0.0f, 0.0f);
  glRotatef(coeff * armStatus.rotation[3] / 2.0f, 0.0f, 0.0f, 1.0f);
  m_Mesh.wheelSmall->Draw(this);
  glRotatef(coeff * armStatus.rotation[3] / 2.0f, 0.0f, 0.0f, 1.0f);
  m_Mesh.smallArm->Draw(this);
  m_Mesh.smallArmCover->Draw(this);

  glTranslatef(25.0f, 0.0f, 0.0f);
  glRotatef(coeff * armStatus.rotation[4], 0.0f, 0.0f, 1.0f);
  m_Mesh.claw->Draw(this);
  m_Mesh.clawCover->Draw(this);
}
