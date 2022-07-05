#include "PlaygroundWidget.h"

#include <windows.h>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>

PlaygroundWidget::PlaygroundWidget(QWidget *parent)
  : QOpenGLWidget(parent),
    m_FrameCount(0)
{
  // Load the WGAPI library.
  HMODULE module = LoadLibraryA("libWGAnimation.dll");
  if (module == nullptr) {
    QMessageBox::critical(nullptr,
                          "致命错误",
                          "无法加载 libWGAnimation.dll\n"
                          "请检查文件是否正确生成");
    QApplication::quit();
    return;
  }

  // Get the WGAPI version.
  VersionFn versionFn = (VersionFn)GetProcAddress(module, "GetWGAPIVersion");
  if (versionFn == nullptr) {
    QMessageBox::critical(nullptr,
                          "致命错误",
                          "无法定位 GetWGAPIVersion 函数\n"
                          "请检查文件是否正确生成");
    QApplication::quit();
    return;
  }
  if (versionFn() > 0x00'01'FFFF) {
    QMessageBox::critical(nullptr,
                          "致命错误",
                          "libWGAnimation.dll 报告的版本与程序不兼容");
    QApplication::quit();
    return;
  }

  // Get the init context function.
  InitContextFn initContextFn =
      (InitContextFn)GetProcAddress(module, "InitContext");
  if (initContextFn == nullptr) {
    QMessageBox::critical(nullptr,
                          "致命错误",
                          "无法定位 InitContext 函数\n"
                          "请检查文件是否正确生成");
    QApplication::quit();
    return;
  }
  m_InitContextFn = initContextFn;

  // Get the destroy context function.
  DestroyContextFn destroyContextFn =
      (DestroyContextFn)GetProcAddress(module, "DestroyContext");
  if (destroyContextFn == nullptr) {
    QMessageBox::critical(nullptr,
                          "致命错误",
                          "无法定位 DestroyContext 函数\n"
                          "请检查文件是否正确生成");
    QApplication::quit();
    return;
  }
  m_DestroyContextFn = destroyContextFn;

  // Get the rewind context function.
  RewindContextFn rewindContextFn =
      (RewindContextFn)GetProcAddress(module, "RewindContext");
  if (rewindContextFn == nullptr) {
    QMessageBox::critical(nullptr,
                          "致命错误",
                          "无法定位 RewindContext 函数\n"
                          "请检查文件是否正确生成");
    QApplication::quit();
    return;
  }
  m_RewindContextFn = rewindContextFn;

  // Get the play animation frame function.
  PlayAnimationFrameFn playAnimationFrameFn =
      (PlayAnimationFrameFn)GetProcAddress(module, "PlayAnimationFrame");
  if (playAnimationFrameFn == nullptr) {
    QMessageBox::critical(nullptr,
                          "致命错误",
                          "无法定位 PlayAnimationFrame 函数\n"
                          "请检查文件是否正确生成");
    QApplication::quit();
    return;
  }
  m_PlayAnimationFrameFn = playAnimationFrameFn;

  // Initialize the context.
  m_Context = initContextFn(this);
  if (m_Context == nullptr) {
    QMessageBox::critical(nullptr, "致命错误", "初始化动画失败");
    QApplication::quit();
    return;
  }

  // Initialize the timer to 90 fps
  QTimer *timer = new QTimer(this);
  timer->setInterval(1000 / 90);

  connect(timer, &QTimer::timeout, this, &PlaygroundWidget::RequestNextFrame);

  setFixedSize(640, 480);
  resize(640, 480);
}

PlaygroundWidget::~PlaygroundWidget() {
  makeCurrent();

  m_DestroyContextFn(m_Context, this);

  doneCurrent();
}

void PlaygroundWidget::initializeGL() {
  initializeOpenGLFunctions();

  glViewport(0, 0, 640, 480);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-320, 320, -240, 240, -1, 1);

  glDisable(GL_LIGHTING);
  glDisable(GL_MULTISAMPLE);
  glDisable(GL_DEPTH_TEST);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void PlaygroundWidget::paintGL() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -0.9f);
  glClear(GL_COLOR_BUFFER_BIT);

  m_PlayAnimationFrameFn(m_Context, this, m_FrameCount);
  m_FrameCount += 1;
}

void PlaygroundWidget::resizeGL(int width, int height) {
  Q_UNUSED(width)
  Q_UNUSED(height)

  glViewport(0, 0, 640, 480);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-320, 320, -240, 240, -1, 1);
}

void PlaygroundWidget::mouseDoubleClickEvent(QMouseEvent *event) {
  Q_UNUSED(event)

  m_FrameCount = 0;

  makeCurrent();
  m_RewindContextFn(m_Context, this);
  doneCurrent();
}

void PlaygroundWidget::RequestNextFrame() {
  update();
}
