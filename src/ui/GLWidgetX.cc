#include "ui/GLWidget.h"
#include "ui/ConfigWidget.h"
#include "util/DynLoad.h"

#include <QDir>
#include <QMessageBox>
#include <QVBoxLayout>

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
    m_StaticScreens.push_back(StaticScreen {
      .imageName = file,
      .texture = std::move(texture)
    });
  }

  emit StaticScreensLoaded(&m_StaticScreens);
}

// directly copied from Qt example
static qreal calculateLevel(QAudioFormat format, const char *data, qint64 len)
{
    const int channelBytes = format.bytesPerSample();
    const int sampleBytes = format.bytesPerFrame();
    if (sampleBytes == 0) {
        return 0.0;
    }
    const int numSamples = len / sampleBytes;

    float maxValue = 0;
    auto *ptr = reinterpret_cast<const unsigned char *>(data);

    for (int i = 0; i < numSamples; ++i) {
        for (int j = 0; j < format.channelCount(); ++j) {
            float value = format.normalizedSampleValue(ptr);

            maxValue = qMax(value, maxValue);
            ptr += channelBytes;
        }
    }
    return maxValue;
}

void GLWidget::InitSoundCapture() {
    QAudioDevice deviceInfo = QMediaDevices::defaultAudioInput();

    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    m_AudioInput.reset(new QAudioSource(deviceInfo, format));
    auto *io = m_AudioInput->start();
    connect(io, &QIODevice::readyRead, [this, format, io] {
        static const qint64 BufferSize = 4096;
        const qint64 len = qMin(m_AudioInput->bytesAvailable(), BufferSize);

        QByteArray buffer(len, 0);
        qint64 l = io->read(buffer.data(), len);
        if (l > 0) {
            const qreal level = calculateLevel(format, buffer.constData(), l);
            m_FaceTrackStatus.PushVolumeSample(level * 1.5);
        }
    });
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
    void *sharedObject = cw::LoadSharedObject(QStringLiteral("animations/dynamic/") + file);
    if (!sharedObject) {
      continue;
    }

    WGAPI_Animation const* animation = cw::LoadSymbol<WGAPI_Animation const*>(
      sharedObject,
      "LoadAnimation"
    );

    m_Animations.emplace_back(std::make_unique<AnimationContext>(animation, sharedObject));
  }
}

void GLWidget::InitAnimations() {
  std::vector<std::unique_ptr<AnimationContext>> initializedAnimations;
  for (auto &animation : m_Animations) {
      if (!animation->Initialize(this)) {
        QMessageBox::warning(
            this,
            "警告",
            QString("动画 %1 初始化失败，将不可用")
              .arg(animation->rawAnimation->name)
        );
        continue;
      }

      initializedAnimations.push_back(std::move(animation));
  }

  m_Animations.swap(initializedAnimations);
  emit AnimationsLoaded(&m_Animations);
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

  emit BodyAnimationsLoaded(&m_BodyAnimations);
}

void GLWidget::RequestNextFrame() {
  if (m_BodyStatus.playAnimationStatus.IsPlayingAnimation()) {
    if (!m_BodyStatus.playAnimationStatus.NextFrame(&m_BodyStatus)) {
      m_BodyStatus.playAnimationStatus.SetAnimation(nullptr);
      emit m_ConfigWidget->DoneBodyAnimation();
    }
  }

  m_FaceTrackStatus.NextFrame();
  m_ScreenStatus.NextFrame();
  m_BodyStatus.NextTick();
  update();
}
