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
  std::size_t smoothSteps = 4;
  float xRotationFix = 0.0f;
  float yRotationFix = 0.0f;
  float zRotationFix = 0.0f;
  float eyeMin = 0.0f;
  float eyeMax = 0.0f;
};

class OSFTrackReceiver : public QObject {
  Q_OBJECT

public:
  OSFTrackReceiver(QObject *parent = nullptr);

  CW_DERIVE_UNCOPYABLE(OSFTrackReceiver)
  CW_DERIVE_UNMOVABLE(OSFTrackReceiver)

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void HeadPoseUpdated(HeadPose headPose);
  void TrackingError(QString reason);
#pragma clang diagnostic pop

public slots:
  void StartListening(std::uint16_t port);
  void StopListening();
  void SetParameter(OSFTrackParameter parameter);

private slots:
  void HandleData(void);

private:
  QUdpSocket *m_Socket;
  OSFTrackParameter m_Parameter;
  std::deque<HeadPose> m_SmoothBuffer;
};

class OSFTrackController : public QWidget {
  Q_OBJECT

public:
  OSFTrackController(FaceTrackStatus *fcs,
                     QWidget *parent = nullptr);
  ~OSFTrackController();

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void StartListening(std::uint16_t port);
  void StopListening(void);
  void SetParameter(OSFTrackParameter parameter);
#pragma clang diagnostic pop

private slots:
  void HandleError(QString reason);
  void HandlePoseUpdate(HeadPose pose);

private:
  QThread m_WorkerThread;
  FaceTrackStatus *m_FCS;
};

#endif // PROJECT_WG_OSFTRACK_H
