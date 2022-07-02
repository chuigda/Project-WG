#include "ui/GLWidget.h"

#include <QDir>
#include <QListWidgetItem>

void GLWidget::LoadStaticScreens() {
  QDir dir(QStringLiteral("animations/static"));
  QStringList filters;
  filters << QStringLiteral("*.bmp");

  QStringList files = dir.entryList(filters, QDir::Files);
  for (const auto &file : files) {
    QImage image;
    image.load(QStringLiteral("animations/static/") + file);

    if (image.isNull()) {
      qWarning() << "Failed to load image" << file;
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
