#ifndef PLAYGROUND_WIDGET_H
#define PLAYGROUND_WIDGET_H

#include <QOpenGLWidget>
#include "cwglx/GL.h"
#include "cwglx/GLImpl.h"

using VersionFn = std::uint32_t (*)(void);
using AnimationNameFn = const char* (*)(void);
using InitContextFn = void* (*)(GLFunctions *f);
using DestroyContextFn = void (*)(void *context, GLFunctions *f);
using RewindContextFn = bool (*)(void *context, GLFunctions *f);
using PlayAnimationFrameFn = bool (*)(void *context,
                                      GLFunctions *f,
                                      std::uint64_t frame);

class PlaygroundWidget final : public QOpenGLWidget, public GLFunctions {
  Q_OBJECT
public:
  explicit PlaygroundWidget(QWidget *parent = nullptr);
  ~PlaygroundWidget() final;

protected:
  void initializeGL() final;
  void paintGL() final;
  void resizeGL(int w, int h) final;

private slots:
  void RequestNextFrame();

private:
  void *m_Context;
  std::uint64_t m_FrameCount;

  InitContextFn m_InitContextFn;
  DestroyContextFn m_DestroyContextFn;
  RewindContextFn m_RewindContextFn;
  PlayAnimationFrameFn m_PlayAnimationFrameFn;
};

#endif // PLAYGROUND_WIDGET_H
