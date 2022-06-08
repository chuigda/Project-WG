#ifndef PRWG_GLWIDGET_H
#define PRWG_GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_2_0>

class GLWidget final : public QOpenGLWidget, public QOpenGLFunctions_2_0 {
  Q_OBJECT

public:
  explicit GLWidget(QWidget *parent = nullptr);
  ~GLWidget() final;

protected:
  void initializeGL() final;
  void paintGL() final;
  void resizeGL(int w, int h) final;
};

#endif // PRWG_GLWIDGET_H
