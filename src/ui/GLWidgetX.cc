#include "ui/GLWidget.h"
#include "ui/ConfigWidget.h"

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
        new Animation(QStringLiteral("animations/dynamic/") + file);
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

      QString originalFileName = anim->GetFileName();
      originalFileName.replace("animations/dynamic/", "", Qt::CaseInsensitive);
      m_AnimationItems.push_back(new AnimationItem(
          anim->GetAnimationName() + " (" + originalFileName + ")",
          anim.get()
      ));
      initializedAnimations.push_back(std::move(anim));
    }
  }
  m_Animations.swap(initializedAnimations);

  emit AnimationsLoaded(&m_AnimationItems);
}

void GLWidget::LoadBodyAnimations() {
  QDir dir(QStringLiteral("animations/body"));
  QStringList filters;
  filters << "*.anim" << "*.txt" << "*.cfg";

  QStringList files = dir.entryList(filters, QDir::Files);
  for (const auto &file : files) {
    std::unique_ptr<wgc0310::BodyAnimation> animation =
      wgc0310::LoadBodyAnimation(
        (QStringLiteral("animations/body/") + file).toStdString().c_str()
      );
    if (!animation) {
      continue;
    }

    m_BodyAnimationNames.push_back(file);
    m_BodyAnimations.push_back(std::move(animation));
  }

  emit BodyAnimationsLoaded(&m_BodyAnimationNames, &m_BodyAnimations);
}

void GLWidget::RequestNextFrame() {
  if (m_BodyStatus.playAnimationStatus.IsPlayingAnimation()) {
    if (!m_BodyStatus.playAnimationStatus.NextFrame(&m_BodyStatus)) {
      m_BodyStatus.playAnimationStatus.SetAnimation(nullptr);
      emit m_ConfigWidget->DoneBodyAnimation();
    }
  }

  m_BodyStatus.NextTick();
  update();
}
