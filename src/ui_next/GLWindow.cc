#include "ui_next/GLWindow.h"

#include <QWindow>
#include <QTimer>
#include <QCloseEvent>
#include <QApplication>
#include <glm/gtc/matrix_transform.hpp>

#include "GlobalConfig.h"
#include "cwglx/Setup.h"
#include "cwglx/GL/GLImpl.h"

GLWindow::GLWindow(EntityStatus const* entityStatus,
                   wgc0310::HeadStatus const* headStatus,
                   wgc0310::BodyStatus const* bodyStatus,
                   wgc0310::AttachmentStatus const* attachmentStatus,
                   wgc0310::ScreenAnimationStatus const *screenAnimationStatus,
                   cw::CircularBuffer<qreal, 160> *volumeLevels,
                   bool *volumeLevelsUpdated,
                   wgc0310::ScreenDisplayMode const *screenDisplayMode,
                   StatusExtra const* statusExtra)
  : QOpenGLWidget(nullptr, Qt::Window),
    GL(new GLFunctions()),
    // Internal status
    m_DevicePixelRatio(1.0),
    // Input status
    m_EntityStatus(entityStatus),
    m_HeadStatus(headStatus),
    m_BodyStatus(bodyStatus),
    m_AttachmentStatus(attachmentStatus),
    m_ScreenAnimationStatus(screenAnimationStatus),
    m_VolumeLevels(volumeLevels),
    m_VolumeLevelsUpdated(volumeLevelsUpdated),
    m_ScreenDisplayMode(screenDisplayMode),
    m_StatusExtra(statusExtra),
    // Internal states and OpenGL resources
    m_Shader(nullptr),
    m_Screen(nullptr),
    m_Projection(1.0f),
    m_PerformanceCounter(0),
    m_PerformanceCounterEnabled(false)
{
  setWindowTitle("Project-WG - 绘图输出窗口");
  setWindowFlags(Qt::CustomizeWindowHint
                 | Qt::WindowTitleHint
                 | Qt::WindowMaximizeButtonHint);

  QSurfaceFormat format = this->format();
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setVersion(3, 3);
  format.setSamples(cw::GlobalConfig::Instance.multisamplingSamples);
  format.setDepthBufferSize(16);
  format.setBlueBufferSize(0);
  format.setGreenBufferSize(0);
  format.setRedBufferSize(0);
  format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

  this->setAttribute(Qt::WA_TranslucentBackground);

  this->setFormat(format);
  this->resize(600, 600);
}

GLWindow::~GLWindow() {
  RunWithGLContext([this] {
    if (m_Shader) {
      m_Shader->Delete(GL);
    }
    if (m_Model) {
      m_Model->Delete(GL);
    }
    if (m_PerformanceCounterEnabled) {
      GL->glDeleteQueries(1, &m_PerformanceCounter);
    }

    delete GL;
  });
}

void GLWindow::RunWithGLContext(std::function<void(void)> const& f) {
  makeCurrent();
  f();
  doneCurrent();
}

void GLWindow::EnablePerformanceCounter() {
  Q_ASSERT(this->isValid());

  m_PerformanceCounterEnabled = true;
  GL->glGenQueries(1, &m_PerformanceCounter);
}

void GLWindow::initializeGL() {
  QOpenGLWidget::initializeGL();
  cw::SetupPreferred(GL);

  if (cw::GlobalConfig::Instance.multisampling) {
    GL->glEnable(GL_MULTISAMPLE);
  } else {
    GL->glDisable(GL_MULTISAMPLE);
  }

  if (cw::GlobalConfig::Instance.lineSmoothHint) {
    GL->glEnable(GL_LINE_SMOOTH);
    GL->glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  } else {
    GL->glDisable(GL_LINE_SMOOTH);
  }

  m_DevicePixelRatio = this->windowHandle()->devicePixelRatio();

  m_Screen = std::make_unique<wgc0310::Screen>(GL);
  ReloadModel();

  emit OpenGLInitialized();
}

void GLWindow::paintGL() {
  if (m_PerformanceCounterEnabled) {
    GL->glBeginQuery(GL_TIME_ELAPSED, m_PerformanceCounter);
  }

  // prepare screen content
  m_Screen->BeginScreenContext(GL);
  // DrawScreenContent();
  m_Screen->DoneScreenContext(GL);

  // switch back to default frame buffer.
  // not using `0` since Qt doesn't use it as the default one.
  GL->glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

  glm::mat4 modelView = glm::identity<glm::mat4x4>();
  modelView = glm::scale(modelView, glm::vec3(0.01f, 0.01f, 0.01f));
  m_EntityStatus->ToMatrix(modelView);

  if (m_StatusExtra->customClearColor) {
    GL->glClearColor(m_StatusExtra->clearColor.r,
                     m_StatusExtra->clearColor.g,
                     m_StatusExtra->clearColor.b,
                     m_StatusExtra->clearColor.a);
  } else {
    GL->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  }
  GL->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_Shader->opaqueShader.UseProgram(GL);
  m_Shader->opaqueShader.SetUniform(GL, QStringLiteral("modelView"), modelView);
  m_Model->testObject.Draw(GL, &m_Shader->opaqueShader);

  if (m_PerformanceCounterEnabled) {
    GL->glEndQuery(GL_TIME_ELAPSED);
  }
}

GLuint64 GLWindow::QueryPerformanceCounter() {
  Q_ASSERT(this->isValid());

  GLuint64 result = 0;
  if (m_PerformanceCounterEnabled) {
    GL->glGetQueryObjectui64v(m_PerformanceCounter, GL_QUERY_RESULT, &result);
  }
  return result;
}

void GLWindow::ReloadModel() {
  if (m_Model) {
    m_Model->Delete(GL);
    m_GLObjectContext.RemoveAll(GL);
  }
  m_Model = std::make_unique<wgc0310::WGCModel>(
    wgc0310::LoadWGCModel(&m_GLObjectContext, GL)
  );
}

bool GLWindow::SetShader(std::unique_ptr<wgc0310::ShaderCollection> &&shader) {
  Q_ASSERT(this->isValid());
  if (m_Shader) {
    m_Shader->Delete(GL);
  }
  m_Shader = std::move(shader);

  m_Shader->emissiveShader.UseProgram(GL);
  m_Shader->emissiveShader.SetUniform(GL, QStringLiteral("projection"), m_Projection);

  m_Shader->translucentShader.UseProgram(GL);
  m_Shader->translucentShader.SetUniform(GL, QStringLiteral("projection"), m_Projection);

  m_Shader->opaqueShader.UseProgram(GL);
  m_Shader->opaqueShader.SetUniform(GL, QStringLiteral("projection"), m_Projection);

  return true;
}

void GLWindow::resizeGL(int w, int h) {
  GL->glViewport(0, 0, w, h);
  m_Projection = glm::perspective<float>(
    glm::radians(45.0f),
    static_cast<float>(w) / static_cast<float>(h),
    0.1f,
    100.0f
  );

  m_Shader->emissiveShader.UseProgram(GL);
  m_Shader->emissiveShader.SetUniform(GL, QStringLiteral("projection"), m_Projection);

  m_Shader->translucentShader.UseProgram(GL);
  m_Shader->translucentShader.SetUniform(GL, QStringLiteral("projection"), m_Projection);

  m_Shader->opaqueShader.UseProgram(GL);
  m_Shader->opaqueShader.SetUniform(GL, QStringLiteral("projection"), m_Projection);
}

void GLWindow::DrawScreenContent() {}
