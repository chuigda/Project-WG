#include "ui/GLWidget.h"

#include <QDir>
#include <QMessageBox>

void GLWidget::LoadAndInitScreens() {
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
        std::make_unique<cw::Texture2D>(image, this);
    cw::Texture2D *ptr = texture.get();
    m_StaticScreens.push_back(std::move(texture));
    m_StaticScreenItems.push_back(new StaticScreenItem(file, ptr));
  }

  emit StaticScreensLoaded(&m_StaticScreenItems);
}

void GLWidget::LoadAnimations() {
  QDir dir(QStringLiteral("animations/dynamic"));
  QStringList filters;
  #ifdef CW_WIN32
    filters << "*.dll";
  #else
    filters << "*.so";
  #endif

  QStringList files = dir.entryList(filters, QDir::Files);
  for (const auto &file : files) {
    Animation *animation =
        new Animation(QStringLiteral("animations/static/") + file);
    if (!animation->IsOpen()) {
      delete animation;
      continue;
    }
    m_Animations.emplace_back(animation);
  }
}

void GLWidget::InitAnimations() {
  std::vector<std::unique_ptr<Animation>> initializedAnimations;

  for (std::unique_ptr<Animation>& ptr : m_Animations) {
    std::unique_ptr<Animation> anim = std::move(ptr);
    if (anim) {
      if (!anim->Initialize(this)) {
        QMessageBox::warning(
            this,
            "警告",
            QString("动画 %1（来自文件 %2）初始化失败，将不可用")
              .arg(anim->GetAnimationName(),
                   anim->GetFileName())
        );
        continue;
      }

      m_AnimationItems.push_back(new AnimationItem(
          anim->GetFileName() + anim->GetAnimationName(),
          anim.get()
      ));
      initializedAnimations.push_back(std::move(anim));
    }
  }
  m_Animations.swap(initializedAnimations);

  emit AnimationsLoaded(&m_AnimationItems);
}
