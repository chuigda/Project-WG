#include "ui_next/SoundControl.h"

#include <QAudioFormat>
#include <QAudioDevice>
#include <QAudioSource>
#include <QMediaDevices>
#include <QMessageBox>
#include <QComboBox>
#include <QProgressBar>
#include <QVBoxLayout>

// directly copied from Qt example
static qreal calculateLevel(QAudioFormat format, const char *data, qint64 len) {
  const int channelBytes = format.bytesPerSample();
  const int sampleBytes = format.bytesPerFrame();
  if (sampleBytes == 0) {
    return 0.0;
  }
  const int numSamples = static_cast<int>(len) / sampleBytes;

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

class SoundAnalysisWorker : public QObject {
  Q_OBJECT

public:
  SoundAnalysisWorker() = default;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void SampleReady(qreal sample);
  void SoundAnalysisError(QString const& reason);
#pragma clang diagnostic pop

public slots:
  void StartAnalysis(QAudioDevice const& device) {
    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    m_AudioSource = std::make_unique<QAudioSource>(device, format);
    if (m_AudioSource->isNull()) {
      emit SoundAnalysisError("启动监听失败");
      m_AudioSource.reset(nullptr);
      return;
    }

    // directly copied from Qt example
    auto *io = m_AudioSource->start();
    connect(io, &QIODevice::readyRead, io, [this, format, io] {
      static const qint64 BufferSize = 4096;
      const qint64 len = qMin(m_AudioSource->bytesAvailable(), BufferSize);

      QByteArray buffer(len, 0);
      qint64 l = io->read(buffer.data(), len);
      if (l > 0) {
        const qreal level = calculateLevel(format, buffer.constData(), l);
        emit SampleReady(level);
      }
    });
  }

  void StopAnalysis() {
    m_AudioSource.reset(nullptr);
  }

private:
  std::unique_ptr<QAudioSource> m_AudioSource;
};

SoundControl::SoundControl(cw::CircularBuffer<qreal, 160> *volumeLevels,
                           bool *volumeLevelsUpdated,
                           QThread *workerThread)
  : m_VolumeLevels(volumeLevels),
    m_VolumeLevelsUpdated(volumeLevelsUpdated),
    m_DeviceSelect(new QComboBox()),
    m_VolumeLevel(new QProgressBar()),
    m_MediaDevices(new QMediaDevices(this)),
    m_WorkerThread(workerThread)
{
  setWindowTitle("音频分析");

  m_VolumeLevel->setMinimum(0);
  m_VolumeLevel->setMaximum(100);
  m_VolumeLevel->setValue(0);
  m_VolumeLevel->setTextVisible(false);

  SoundAnalysisWorker *worker = new SoundAnalysisWorker();
  worker->moveToThread(m_WorkerThread);

  connect(this, &SoundControl::StartAnalysis, worker, &SoundAnalysisWorker::StartAnalysis);
  connect(this, &SoundControl::StopAnalysis, worker, &SoundAnalysisWorker::StopAnalysis);
  connect(worker, &SoundAnalysisWorker::SampleReady, this, &SoundControl::HandleSample);
  connect(worker, &SoundAnalysisWorker::SoundAnalysisError, this, &SoundControl::HandleError);

  connect(m_MediaDevices, &QMediaDevices::audioInputsChanged, this, &SoundControl::ReloadAudioDevices);
  connect(m_MediaDevices, &QMediaDevices::audioInputsChanged, this, &SoundControl::StopAnalysis);

  connect(m_DeviceSelect, &QComboBox::currentIndexChanged, this, [this] (int index) {
    if (index == 0) {
      emit StopAnalysis();
    } else {
      QAudioDevice const& device = m_DetectedAudioDevices[index - 1];
      emit StartAnalysis(device);
    }
  });

  ReloadAudioDevices();

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(m_DeviceSelect);
  layout->addWidget(m_VolumeLevel);
  setLayout(layout);

  setFixedWidth(400);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "VirtualCallInCtorOrDtor"
  // This should be safe since base class `QWidget` has been initialised,
  // and we did not reimplement `sizeHint`
  setFixedHeight(sizeHint().height());
#pragma clang diagnostic pop
}

void SoundControl::ReloadAudioDevices() {
  m_DeviceSelect->blockSignals(true);

  m_DetectedAudioDevices = QMediaDevices::audioInputs();
  m_DeviceSelect->clear();
  m_DeviceSelect->addItem("停用");
  for (const auto & device : m_DetectedAudioDevices) {
    m_DeviceSelect->addItem(device.description());
  }

  m_DeviceSelect->blockSignals(false);
  m_DeviceSelect->setCurrentIndex(0);
}

void SoundControl::HandleSample(qreal sample) {
  m_VolumeLevel->setValue(static_cast<int>(sample * 100.0));
  m_VolumeLevels->PopFront();
  m_VolumeLevels->PushBack(sample * 1.5);
  *m_VolumeLevelsUpdated = true;
}

void SoundControl::HandleError(const QString &reason) {
  QMessageBox::warning(this, "音频分析错误", reason);
  m_DeviceSelect->setCurrentIndex(0);
}

#include "SoundControl.moc"
