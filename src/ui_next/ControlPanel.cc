#include "ui_next/ControlPanel.h"
#include "ui/GLWidget.h"

ControlPanel::ControlPanel()
  : QWidget(nullptr, Qt::Window),
    m_GLWidget(new GLWidget()),
    m_OpenGLSettingsButton(nullptr),
    m_CameraSettingsButton(nullptr),
    m_BodyAnimationButton(nullptr),
    m_FaceAnimationButton(nullptr),
    m_PoseEstimationButton(nullptr),
    m_AboutButton(nullptr)
{
  m_GLWidget->show();
}
