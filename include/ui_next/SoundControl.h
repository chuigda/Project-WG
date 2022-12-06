#ifndef PROJECT_WG_SOUND_CONTROL_H
#define PROJECT_WG_SOUND_CONTROL_H

#include <QList>
#include <QAudioDevice>

#include "ui_next/CloseSignallingWidget.h"
#include "util/CircularBuffer.h"

class QMediaDevices;
class QComboBox;
class QProgressBar;

class SoundControl : public CloseSignallingWidget {
  Q_OBJECT

public:
  SoundControl(cw::CircularBuffer<qreal, 160> *volumeLevels,
               bool *volumeLevelsUpdated,
               QThread *workerThread);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void StartAnalysis(QAudioDevice const& device);
  void StopAnalysis();
#pragma clang diagnostic pop

private slots:
  void ReloadAudioDevices();
  void HandleSample(qreal sample);
  void HandleError(QString const& reason);

private:
  cw::CircularBuffer<qreal, 160> *m_VolumeLevels;
  bool *m_VolumeLevelsUpdated;

  QComboBox *m_DeviceSelect;
  QProgressBar *m_VolumeLevel;
  QList<QAudioDevice> m_DetectedAudioDevices;

  QMediaDevices *m_MediaDevices;
  QThread *m_WorkerThread;
};

#endif // PROJECT_WG_SOUND_CONTROL_H
