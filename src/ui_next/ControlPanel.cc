#include "ui_next/ControlPanel.h"
#include "ui/GLWidget.h"

#include <QPushButton>
#include <QVBoxLayout>

ControlPanel::ControlPanel()
  : QWidget(nullptr, Qt::Window),
    m_GLWidget(new GLWidget()),
    m_OpenGLSettingsButton(new QPushButton("OpenGL 设定")),
    m_CameraSettingsButton(new QPushButton("相机设置")),
    m_BodyAnimationButton(new QPushButton("关节动画")),
    m_FaceAnimationButton(new QPushButton("面部动画")),
    m_PoseEstimationButton(new QPushButton("姿态控制")),
    m_AboutButton(new QPushButton("关于"))
{
  m_GLWidget->show();

  m_OpenGLSettingsButton->setCheckable(true);
  m_CameraSettingsButton->setCheckable(true);
  m_BodyAnimationButton->setCheckable(true);
  m_FaceAnimationButton->setCheckable(true);
  m_PoseEstimationButton->setCheckable(true);
  m_AboutButton->setCheckable(true);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_OpenGLSettingsButton);
  layout->addWidget(m_CameraSettingsButton);
  layout->addWidget(m_BodyAnimationButton);
  layout->addWidget(m_FaceAnimationButton);
  layout->addWidget(m_PoseEstimationButton);
  layout->addWidget(m_AboutButton);
  layout->addStretch();

  this->setLayout(layout);
}
