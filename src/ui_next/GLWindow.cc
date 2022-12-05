#include "ui_next/GLWindow.h"

#include <QTimer>
#include "cwglx/Setup.h"
#include "cwglx/drawable/TriangleGen.h"
#include "wgc0310/ScreenGlass.h"
#include "cwglx/drawable/PlainTriangles.h"
#include "glu/FakeGLU.h"

GLWindow::GLWindow(CameraEntityStatus const* cameraEntityStatus,
                   wgc0310::HeadStatus const* headStatus,
                   cw::CircularBuffer<qreal, 160> *volumeLevels,
                   bool *volumeLevelsUpdated,
                   wgc0310::ScreenDisplayMode const *screenDisplayMode)
  : QOpenGLWidget(nullptr, Qt::Window),
    // Input status
    m_CameraEntityStatus(cameraEntityStatus),
    m_HeadStatus(headStatus),
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
    m_VolumeVBO { 0, 0 },
    // Event-based timer
    m_Timer(new QTimer(this))
{
  setWindowTitle("Project-WG - 绘图输出窗口");

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

  // TODO

  emit OpenGLInitialized();
}

void GLWindow::paintGL() {
  // prepare screen content
  m_Screen->BeginScreenContext(this);
  // TODO migrate screen content drawing code
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

  m_CameraEntityStatus->ApplyCameraTransformation(this);
  m_Light->Enable(this);
  m_Light2->Enable(this);
  m_CameraEntityStatus->ApplyEntityTransformation(this);

  // TODO
}

void GLWindow::resizeGL(int w, int h) {
  Q_UNUSED(w)
  Q_UNUSED(h)
}
