#include "ui_next/ScreenAnimationControl.h"

#include <QDir>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include "wgc0310/ScreenAnimationStatus.h"
#include "ui_next/GLWindow.h"
#include "util/DynLoad.h"

ScreenAnimationControl::ScreenAnimationControl(GLWindow *glWindow,
                                               wgc0310::ScreenAnimationStatus *animationStatus)
  : QWidget(nullptr, Qt::Window),
    m_GLWindow(glWindow),
    m_ScreenAnimationStatus(animationStatus)
{
  connect(m_GLWindow, &GLWindow::OpenGLInitialized,
          this, &ScreenAnimationControl::GLContextReady);

  QWidget *minimalDisplayWidget = new QWidget();
  QWidget *detailedDisplayWidget = new QWidget();
  detailedDisplayWidget->setVisible(false);

  QVBoxLayout *mainLayout = new QVBoxLayout();
  this->setLayout(mainLayout);
  mainLayout->addWidget(minimalDisplayWidget);
  mainLayout->addWidget(detailedDisplayWidget);

  {
    QHBoxLayout *layout = new QHBoxLayout();
    mainLayout->addLayout(layout);

    layout->addWidget(new QLabel("显示模式"));
    layout->addStretch();
    QRadioButton *minimalDisplayMode = new QRadioButton("简单");
    minimalDisplayMode->setChecked(true);
    layout->addWidget(minimalDisplayMode);
    QRadioButton *detailedDisplayMode = new QRadioButton("详细");
    layout->addWidget(detailedDisplayMode);

    connect(
      minimalDisplayMode,
      &QRadioButton::toggled,
      minimalDisplayMode,
      [minimalDisplayWidget] (bool toggled) {
        minimalDisplayWidget->setVisible(toggled);
      }
    );

    connect(
      detailedDisplayMode,
      &QRadioButton::toggled,
      detailedDisplayMode,
      [detailedDisplayWidget] (bool toggled) {
        detailedDisplayWidget->setVisible(toggled);
      }
    );
  }

  auto reloadStaticImages = [this] {
    m_ScreenAnimationStatus->Reset();
    m_GLWindow->RunWithGLContext([this] {
      this->ReloadStaticImages();
    });
  };

  auto reloadAnimations = [this] {
    m_ScreenAnimationStatus->Reset();
    m_GLWindow->RunWithGLContext([this] {
      this->ReloadScreenAnimations();
    });
  };

  // 最小化显示模式
  {
    QVBoxLayout *layout = new QVBoxLayout();
    minimalDisplayWidget->setLayout(layout);

    // 静态画面
    {
      QGroupBox *staticImageGroup = new QGroupBox("静态图像");
      QHBoxLayout *hBox = new QHBoxLayout();
      staticImageGroup->setLayout(hBox);

      hBox->addLayout(m_StaticImageButtonsLayout);
      hBox->addStretch();
      QPushButton *reloadButton = new QPushButton("↻");
      reloadButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      connect(reloadButton, &QPushButton::clicked, reloadButton, reloadStaticImages);
      hBox->addWidget(reloadButton);

      layout->addWidget(staticImageGroup);
    }

    // 动画
    {
      QGroupBox *animationsGroup = new QGroupBox("动画");
      QHBoxLayout *hBox = new QHBoxLayout();
      animationsGroup->setLayout(hBox);

      hBox->addLayout(m_ScreenAnimationButtonsLayout);
      hBox->addStretch();
      QPushButton *controlButton = new QPushButton("⌘");
      controlButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      hBox->addWidget(controlButton);
      QPushButton *reloadButton = new QPushButton("↻");
      reloadButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      connect(reloadButton, &QPushButton::clicked, reloadButton, reloadAnimations);
      hBox->addWidget(reloadButton);

      layout->addWidget(animationsGroup);
    }
  }

  // 常规显示模式
  {
    QHBoxLayout *layout = new QHBoxLayout();
    detailedDisplayWidget->setLayout(layout);

    {
      QGroupBox *staticImageGroup = new QGroupBox("静态图像");
      QVBoxLayout *vBox = new QVBoxLayout();
      staticImageGroup->setLayout(vBox);

      vBox->addLayout(m_StaticImageButtonsLayoutV);
      vBox->addStretch();
      QPushButton *reloadButton = new QPushButton("重新加载动画");
      connect(reloadButton, &QPushButton::clicked, reloadButton, reloadStaticImages);
      vBox->addWidget(reloadButton);

      layout->addWidget(staticImageGroup);
    }

    {
      QGroupBox *staticImageGroup = new QGroupBox("动画");
      QVBoxLayout *vBox = new QVBoxLayout();
      staticImageGroup->setLayout(vBox);

      vBox->addLayout(m_ScreenAnimationButtonsLayoutV);
      vBox->addStretch();
      QPushButton *controlButton = new QPushButton("动画控制");
      vBox->addWidget(controlButton);
      QPushButton *reloadButton = new QPushButton("重新加载动画");
      connect(reloadButton, &QPushButton::clicked, reloadButton, reloadStaticImages);
      vBox->addWidget(reloadButton);

      layout->addWidget(staticImageGroup);
    }
  }
}

void ScreenAnimationControl::GLContextReady() {
  m_ScreenAnimationStatus->Reset();
  m_GLWindow->RunWithGLContext([this] {
    ReloadStaticImages();
    ReloadScreenAnimations();
  });
}

static void ClearLayout(QLayout *layout) {
  while (QLayoutItem *child = layout->takeAt(0))  {
    delete child->widget();
    delete child;
  }
}

void ScreenAnimationControl::ReloadStaticImages() {
  for (const auto &image : m_StaticImages) {
    image.texture->DeleteTexture(m_GLWindow);
  }
  m_StaticImages.clear();

  QDir dir(QStringLiteral("animations/static"));
  QStringList filters;
  filters << QStringLiteral("*.bmp");

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

    std::unique_ptr<cw::Texture2D> texture =
      std::make_unique<cw::Texture2D>(image, m_GLWindow);
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
      hButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      hButton->setToolTip(image.imageName);
      QPushButton *vButton = new QPushButton(image.imageName);

      auto clickHandler = [this, imagePtr] {
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
  for (const auto &m_Animation : m_ScreenAnimations) {
    m_Animation->Delete(m_GLWindow);
  }
  m_ScreenAnimations.clear();

  QDir dir(QStringLiteral("animations/dynamic"));
  QStringList filters;
  #ifdef CW_WIN32
    filters << "*.dll";
  #else
    filters << "*.so";
  #endif

  QStringList files = dir.entryList(filters, QDir::Files);
  std::vector<std::unique_ptr<wgc0310::ScreenAnimation>> screenAnimations;

  for (const auto &file : files) {
    void *sharedObject = cw::LoadSharedObject(QStringLiteral("animations/dynamic/") + file);
    if (!sharedObject) {
      continue;
    }

    LoadAnimationFn loadAnimationFn = cw::TryReadSymbol<LoadAnimationFn>(
      sharedObject,
      "LoadAnimation"
    );
    if (!loadAnimationFn) {
      continue;
    }

    WGAPI_Animation const *animation = loadAnimationFn();
    if (!animation || animation->version != WGAPI_VERSION) {
      continue;
    }

    m_ScreenAnimations.emplace_back(std::make_unique<wgc0310::ScreenAnimation>(
      animation,
      sharedObject
    ));
  }

  ClearLayout(m_ScreenAnimationButtonsLayout);
  ClearLayout(m_ScreenAnimationButtonsLayoutV);
  {
    int i = 0;
    for (auto &animation : m_ScreenAnimations) {
      wgc0310::ScreenAnimation *animationPtr = animation.get();

      QPushButton *hButton = new QPushButton(QString::number(i));
      hButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      QString animationName(animation->rawAnimation->name);
      hButton->setToolTip(animationName);
      QPushButton *vButton = new QPushButton(animationName);

      auto clickHandler = [this, animationPtr] {
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
