#ifndef PROJECT_WG_OSFTRACK_H
#define PROJECT_WG_OSFTRACK_H

#include <QWidget>
#include <QThread>
#include <cstdlib>
#include <deque>

#include "ui/FaceTrackStatus.h"
#include "util/Derive.h"

class QUdpSocket;

struct OSFTrackParameter {
  std::size_t smoothSteps = 8;
  float xRotationFix = 0.0f;
  float yRotationFix = 0.0f;
  float zRotationFix = 0.0f;

  std::size_t eyeSmoothSteps = 3;
  float eyeMin = 0.0f;
  float eyeMax = 0.0f;
};

class OSFTrackReceiver : public QObject {
  Q_OBJECT

public:
  explicit OSFTrackReceiver(QObject *parent = nullptr);

  CW_DERIVE_UNCOPYABLE(OSFTrackReceiver)
  CW_DERIVE_UNMOVABLE(OSFTrackReceiver)

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void HeadPoseUpdated(HeadPose headPose);
  void TrackingError(QString const& reason);
#pragma clang diagnostic pop

public slots:
  void StartListening(std::uint16_t port);
  void StopListening();
  void SetParameter(OSFTrackParameter parameter);

private slots:
  void HandleData();

private:
  QUdpSocket *m_Socket;
  OSFTrackParameter m_Parameter;
  std::deque<HeadPose> m_SmoothBuffer;
};

class OSFTrackController final : public QWidget {
  Q_OBJECT

public:
  explicit OSFTrackController(FaceTrackStatus *fcs, QWidget *parent = nullptr);
  ~OSFTrackController() final;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void StartListening(std::uint16_t port);
  void StopListening();
  void SetParameter(OSFTrackParameter parameter);
#pragma clang diagnostic pop

private slots:
  void HandleError(const QString& reason);
  void HandlePoseUpdate(HeadPose pose);

private:
  QThread m_WorkerThread;
  FaceTrackStatus *m_FaceTrackStatus;
};

#endif // PROJECT_WG_OSFTRACK_H
