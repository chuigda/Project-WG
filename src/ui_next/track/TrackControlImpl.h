#ifndef PROJECT_WG_UINEXT_TRACK_CONTROL_IMPL_H
#define PROJECT_WG_UINEXT_TRACK_CONTROL_IMPL_H

#include "ui_next/FaceTrackControl.h"
#include <QWidget>

class QLabel;

class VTSTrackControl : public QWidget {
  Q_OBJECT

public:
  VTSTrackControl(wgc0310::HeadStatus *headStatus,
                  QThread *workerThread,
                  QWidget *parent = nullptr);

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void StartTracking(quint16 port);
  void StopTracking();
#pragma clang diagnostic pop

public slots:
  void HandleError(const QString& error);
  void HandleHeadStatus(wgc0310::HeadStatus headStatus);

private:
  wgc0310::HeadStatus *m_HeadStatus;
  QThread *m_WorkerThread;
};

struct OSFTrackParameter2 {
  std::size_t smoothSteps = 8;
  float xRotationFix = 0.0f;
  float yRotationFix = 0.0f;
  float zRotationFix = 0.0f;

  float eyeMin = -1.0f;
  float eyeMax = 0.3f;
};

class OSFTrackControl final : public QWidget {
  Q_OBJECT

public:
  OSFTrackControl(wgc0310::HeadStatus *headStatus,
                  QThread *workerThread,
                  QWidget *parent = nullptr);
  ~OSFTrackControl() noexcept final;

signals:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
  void StartTracking(quint16 port);
  void StopTracking();
  void SetParameters(OSFTrackParameter2 parameter);
#pragma clang diagnostic pop

public slots:
  void HandleError(const QString& error);
  void HandleHeadStatus(wgc0310::HeadStatus headStatus);

private:
  wgc0310::HeadStatus *m_HeadStatus;
  QThread *m_WorkerThread;
};

class MPTrackControl final : public QWidget {
  Q_OBJECT

public:

public slots:
  void HandleError(const QString& error);
  void HandleHeadStatus(wgc0310::HeadStatus headStatus);

private:
  wgc0310::HeadStatus *m_HeadStatus;
  wgc0310::BodyStatus *m_BodyStatus;
  QThread *m_WorkerThread;
};

class ManualTrackControl : public QWidget {
  Q_OBJECT

public:
  ManualTrackControl(wgc0310::HeadStatus *headStatus,
                     wgc0310::ScreenDisplayMode *screenDisplayMode,
                     QWidget *parent = nullptr);

private:
  wgc0310::HeadStatus *m_HeadStatus;
  wgc0310::ScreenDisplayMode *m_ScreenDisplayMode;
};

#endif
