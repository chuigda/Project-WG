#ifndef PRWG_GLWIDGET_H
#define PRWG_GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_1_5>

class GLWidget final : public QOpenGLWidget, public QOpenGLFunctions_1_5 {
  Q_OBJECT

public:
  GLWidget(QWidget *parent = nullptr);
  ~GLWidget();

protected:
  void initializeGL() override final;
  void paintGL() override final;
  void resizeGL(int w, int h) override final;
};

#endif // PRWG_GLWIDGET_H
