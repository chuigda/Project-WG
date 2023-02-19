#include "ui_next/ScreenAnimationControl.h"

#include <QDir>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>

#include "GlobalConfig.h"
#include "cwglx/Base/Texture.h"
#include "wgc0310/ScreenAnimationStatus.h"
#include "ui_next/GLWindow.h"
#include "util/DynLoad.h"

ScreenAnimationControl::ScreenAnimationControl(GLWindow *glWindow,
                                               wgc0310::ScreenAnimationStatus *animationStatus,
                                               wgc0310::ScreenDisplayMode *screenDisplayMode,
                                               StatusExtra *statusExtra)
  : CloseSignallingWidget(nullptr, Qt::Window),
    m_GLWindow(glWindow),
    m_ScreenAnimationStatus(animationStatus),
    m_ScreenDisplayMode(screenDisplayMode),
    m_StatusExtra(statusExtra),
    m_StaticImageButtonsLayout(new QHBoxLayout()),
    m_StaticImageButtonsLayoutV(new QVBoxLayout()),
    m_ScreenAnimationButtonsLayout(new QHBoxLayout()),
    m_ScreenAnimationButtonsLayoutV(new QVBoxLayout()) {
  setWindowTitle("屏幕画面");
  setMinimumWidth(600);

  connect(m_GLWindow,
          &GLWindow::OpenGLInitialized,
          this,
          &ScreenAnimationControl::GLContextReady);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  this->setLayout(mainLayout);

  {
    QHBoxLayout *layout = new QHBoxLayout();
    mainLayout->addLayout(layout);

    layout->addWidget(new QLabel("显示模式"));
    layout->addStretch();

    m_PlayingCapturedExpression = new QRadioButton("表情捕获");
    m_PlayingSoundWave = new QRadioButton("音频信号");
    m_PlayingStaticImage = new QRadioButton("静态画面");
    m_PlayingDynamicAnimation = new QRadioButton("动画");
    m_PlayingCapturedExpression->setChecked(true);
    m_PlayingStaticImage->setDisabled(true);
    m_PlayingDynamicAnimation->setDisabled(true);
    layout->addWidget(m_PlayingCapturedExpression);
    layout->addWidget(m_PlayingSoundWave);
    layout->addWidget(m_PlayingStaticImage);
    layout->addWidget(m_PlayingDynamicAnimation);

    connect(
      m_PlayingCapturedExpression,
      &QRadioButton::toggled,
      this,
      [this](bool toggled) {
        if (toggled) {
          m_ScreenAnimationStatus->Reset();
          *m_ScreenDisplayMode = wgc0310::ScreenDisplayMode::CapturedExpression;
        }
      }
    );

    connect(
      m_PlayingSoundWave,
      &QRadioButton::toggled,
      this,
      [this](bool toggled) {
        if (toggled) {
          m_ScreenAnimationStatus->Reset();
          *m_ScreenDisplayMode = wgc0310::ScreenDisplayMode::SoundWave;
        }
      }
    );
  }

  auto reloadStaticImages = [this] {
    m_ScreenAnimationStatus->Reset();
    if (*m_ScreenDisplayMode == wgc0310::ScreenDisplayMode::SoundWave) {
      m_PlayingSoundWave->setChecked(true);
    } else {
      m_PlayingCapturedExpression->setChecked(true);
    }
    m_GLWindow->RunWithGLContext([this] {
      this->ReloadStaticImages();
    });
  };

  QGroupBox *staticImageMinimized = new QGroupBox("静态图像");
  QGroupBox *staticImageDetailed = new QGroupBox("静态图像");
  staticImageDetailed->hide();

  {
    QHBoxLayout *hBox = new QHBoxLayout();
    staticImageMinimized->setLayout(hBox);

    hBox->addLayout(m_StaticImageButtonsLayout);
    hBox->addStretch();
    QPushButton *reloadButton = new QPushButton("↻");
    reloadButton->setFixedWidth(32);
    connect(reloadButton, &QPushButton::clicked, reloadButton, reloadStaticImages);
    hBox->addWidget(reloadButton);

    QPushButton *expandButton = new QPushButton("▼");
    expandButton->setFixedWidth(32);
    connect(expandButton, &QPushButton::clicked, this, [staticImageMinimized, staticImageDetailed] {
      staticImageMinimized->hide();
      staticImageDetailed->show();
    });
    hBox->addWidget(expandButton);

    mainLayout->addWidget(staticImageMinimized);
  }

  {
    QVBoxLayout *vBox = new QVBoxLayout();
    staticImageDetailed->setLayout(vBox);

    vBox->addLayout(m_StaticImageButtonsLayoutV);
    vBox->addStretch();
    QPushButton *reloadButton = new QPushButton("↻ 重新加载");
    connect(reloadButton, &QPushButton::clicked, reloadButton, reloadStaticImages);
    vBox->addWidget(reloadButton);

    QPushButton *retractButton = new QPushButton("▲ 收起细节");
    vBox->addWidget(retractButton);
    connect(retractButton, &QPushButton::clicked, this, [staticImageMinimized, staticImageDetailed] {
      staticImageMinimized->show();
      staticImageDetailed->hide();
    });

    mainLayout->addWidget(staticImageDetailed);
  }

  auto reloadAnimations = [this] {
    m_ScreenAnimationStatus->Reset();
    if (*m_ScreenDisplayMode == wgc0310::ScreenDisplayMode::SoundWave) {
      m_PlayingSoundWave->setChecked(true);
    } else {
      m_PlayingCapturedExpression->setChecked(true);
    }
    m_GLWindow->RunWithGLContext([this] {
      this->ReloadScreenAnimations();
    });
  };

  QGroupBox *animationMinimized = new QGroupBox("动画");
  QGroupBox *animationDetailed = new QGroupBox("动画");
  animationDetailed->hide();

  auto controlAnimation = [this] {
    if (m_ScreenAnimationStatus->animation) {
      QWidget *controlWidget = m_ScreenAnimationStatus->animation->GetControlWidget();
      if (controlWidget) {
        controlWidget->setWindowFlag(Qt::WindowStaysOnTopHint, m_StatusExtra->stayOnTop);
        controlWidget->show();
      } else {
        QMessageBox::information(this, "信息", "这个动画并没有控制组件");
      }
    }
  };

  {
    QHBoxLayout *hBox = new QHBoxLayout();
    animationMinimized->setLayout(hBox);

    hBox->addLayout(m_ScreenAnimationButtonsLayout);
    hBox->addStretch();

    QPushButton *configButton = new QPushButton("⌘");
    configButton->setFixedWidth(32);
    connect(configButton, &QPushButton::clicked, configButton, controlAnimation);
    hBox->addWidget(configButton);

    QPushButton *reloadButton = new QPushButton("↻");
    reloadButton->setFixedWidth(32);
    connect(reloadButton, &QPushButton::clicked, reloadButton, reloadAnimations);
    hBox->addWidget(reloadButton);

    QPushButton *expandButton = new QPushButton("▼");
    expandButton->setFixedWidth(32);
    connect(expandButton, &QPushButton::clicked, this, [animationMinimized, animationDetailed] {
      animationMinimized->hide();
      animationDetailed->show();
    });
    hBox->addWidget(expandButton);

    mainLayout->addWidget(animationMinimized);
  }

  {
    QVBoxLayout *vBox = new QVBoxLayout();
    animationDetailed->setLayout(vBox);

    vBox->addLayout(m_ScreenAnimationButtonsLayoutV);
    vBox->addStretch();

    QPushButton *configButton = new QPushButton("⌘ 配置");
    connect(configButton, &QPushButton::clicked, configButton, controlAnimation);
    vBox->addWidget(configButton);

    QPushButton *reloadButton = new QPushButton("↻ 重新加载");
    connect(reloadButton, &QPushButton::clicked, reloadButton, reloadAnimations);
    vBox->addWidget(reloadButton);

    QPushButton *retractButton = new QPushButton("▲ 收起细节");
    vBox->addWidget(retractButton);
    connect(retractButton, &QPushButton::clicked, this, [animationMinimized, animationDetailed] {
      animationMinimized->show();
      animationDetailed->hide();
    });

    mainLayout->addWidget(animationDetailed);
  }
}

void ScreenAnimationControl::GLContextReady() {
  m_ScreenAnimationStatus->Reset();

  ReloadStaticImages();
  ReloadScreenAnimations();
}

static void ClearLayout(QLayout *layout) {
  while (QLayoutItem *child = layout->takeAt(0))  {
    delete child->widget();
    delete child;
  }
}

void ScreenAnimationControl::ReloadStaticImages() {
  for (const auto &image : m_StaticImages) {
    image.texture->Delete(m_GLWindow->GL);
  }
  m_StaticImages.clear();

  QDir dir(QStringLiteral("animations/static"));
  QStringList filters;
  filters << QStringLiteral("*.bmp")
          << QStringLiteral("*.png");

  QStringList files = dir.entryList(filters, QDir::Files);
  for (const auto &file : files) {
    QString fileName = QStringLiteral("animations/static/") + file;

    QImage image;
    image.load(fileName);
    if (image.isNull()) {
      QMessageBox::warning(
        this,
        "警告",
        QString("加载图片文件 %1 失败，该图片将不可用\n")
          .arg(fileName));
      continue;
    }

    auto texture = std::make_unique<cw::Texture2D>(
      image,
      m_GLWindow->GL,
      cw::GlobalConfig::Instance.linearSampling,
      cw::GlobalConfig::Instance.anisotropyFilter
    );
    m_StaticImages.push_back(wgc0310::StaticScreenImage {
      .imageName = file,
      .texture = std::move(texture)
    });
  }

  ClearLayout(m_StaticImageButtonsLayout);
  ClearLayout(m_StaticImageButtonsLayoutV);
  {
    int i = 0;
    for (auto &image: m_StaticImages) {
      wgc0310::StaticScreenImage *imagePtr = &image;

      QPushButton *hButton = new QPushButton(QString::number(i));
      hButton->setFixedWidth(32);
      hButton->setToolTip(image.imageName);
      QPushButton *vButton = new QPushButton(image.imageName);

      auto clickHandler = [this, imagePtr] {
        m_PlayingStaticImage->setChecked(true);
        m_ScreenAnimationStatus->PlayStaticAnimation(imagePtr);
      };

      connect(hButton, &QPushButton::clicked, hButton, clickHandler);
      connect(vButton, &QPushButton::clicked, vButton, clickHandler);
      m_StaticImageButtonsLayout->addWidget(hButton);
      m_StaticImageButtonsLayoutV->addWidget(vButton);
      i += 1;
    }
  }
}

void ScreenAnimationControl::ReloadScreenAnimations() {
  for (const auto &animation : m_ScreenAnimations) {
    animation->Delete(m_GLWindow->GL);
  }
  m_ScreenAnimations.clear();
  for (const auto &sharedObject : m_SharedObjects) {
    cw::DetachSharedObject(sharedObject);
  }
  m_SharedObjects.clear();

  QDir dir(QStringLiteral("animations/dynamic"));
  QStringList filters;
  #ifdef CW_WIN32
    filters << "*.dll";
  #else
    filters << "*.so";
  #endif

  QStringList files = dir.entryList(filters, QDir::Files);
  for (const auto &file : files) {
    QString filePath = QStringLiteral("animations/dynamic/") + file;
    void *sharedObject = cw::LoadSharedObject(filePath);
    if (!sharedObject) {
      QMessageBox::warning(
        this,
        "动画加载错误",
        QString("无法打开共享对象 %1").arg(filePath)
      );
      continue;
    }

    wgc0310::CheckVersionFn checkVersionFn = cw::TryReadSymbol<wgc0310::CheckVersionFn>(
      sharedObject,
      "GetWGAPIVersion"
    );
    if (!checkVersionFn) {
      QMessageBox::warning(
        this,
        "动画加载错误",
        QString("无法在共享对象 %1 上定位 <code>GetWGAPIVersion</code>")
          .arg(filePath)
      );
      cw::DetachSharedObject(sharedObject);
      continue;
    }

    if (checkVersionFn() != WGAPI_VERSION) {
      QMessageBox::warning(
        this,
        "动画加载错误",
        QString("共享对象 %1 使用的 <code>WGAPI</code> 版本与 Project-WG 程序不匹配")
          .arg(filePath)
      );
      cw::DetachSharedObject(sharedObject);
      continue;
    }

    wgc0310::LoadAnimationFn loadAnimationFn = cw::TryReadSymbol<wgc0310::LoadAnimationFn>(
      sharedObject,
      "LoadAnimation"
    );
    if (!loadAnimationFn) {
      QMessageBox::warning(
        this,
        "动画加载错误",
        QString("无法在共享对象 %1 上定位 <code>LoadAnimation</code>")
          .arg(filePath)
      );
      cw::DetachSharedObject(sharedObject);
      continue;
    }

    wgc0310::WGAPIAnimation *animation = loadAnimationFn();
    if (!animation) {
      QMessageBox::warning(
        this,
        "动画加载错误",
        QString("共享对象 %1 的 <code>LoadAnimation</code> 返回了 <code>nullptr</code>")
          .arg(filePath)
      );
      cw::DetachSharedObject(sharedObject);
      continue;
    }

    animation->Initialize(m_GLWindow->GL);

    m_ScreenAnimations.emplace_back(animation);
    m_SharedObjects.push_back(sharedObject);

    QWidget *controlWidget = animation->GetControlWidget();
    if (controlWidget) {
      connect(this, &CloseSignallingWidget::AboutToClose, controlWidget, &QWidget::close);
    }
  }

  ClearLayout(m_ScreenAnimationButtonsLayout);
  ClearLayout(m_ScreenAnimationButtonsLayoutV);
  {
    int i = 0;
    for (auto &animation : m_ScreenAnimations) {
      wgc0310::WGAPIAnimation *animationPtr = animation.get();

      QPushButton *hButton = new QPushButton(QString::number(i));
      hButton->setFixedWidth(32);
      QString animationName(animation->GetName());
      hButton->setToolTip(animationName);
      QPushButton *vButton = new QPushButton(animationName);

      auto clickHandler = [this, animationPtr] {
        m_PlayingDynamicAnimation->setChecked(true);
        m_ScreenAnimationStatus->PlayAnimation(animationPtr);
      };

      connect(hButton, &QPushButton::clicked, hButton, clickHandler);
      connect(vButton, &QPushButton::clicked, vButton, clickHandler);
      m_ScreenAnimationButtonsLayout->addWidget(hButton);
      m_ScreenAnimationButtonsLayoutV->addWidget(vButton);
      i += 1;
    }
  }
}
