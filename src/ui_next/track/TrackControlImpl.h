#ifndef PROJECT_WG_UINEXT_TRACK_CONTROL_IMPL_H
#define PROJECT_WG_UINEXT_TRACK_CONTROL_IMPL_H

#include "ui_next/FaceTrackControl.h"
#include <QWidget>

class VTSTrackControl : public QWidget {
  Q_OBJECT

public:
  VTSTrackControl(wgc0310::HeadStatus *headStatus,
                  QThread *workerThread,
                  QWidget *parent = nullptr);

private:
  wgc0310::HeadStatus *m_HeadStatus;
  QThread *m_WorkerThread;
};

class OSFTrackControl : public QWidget {
  Q_OBJECT

public:
  OSFTrackControl(wgc0310::HeadStatus *headStatus,
                  QThread *workerThread,
                  QWidget *parent = nullptr);

  void StopTracking();

private:
  wgc0310::HeadStatus *m_HeadStatus;
  QThread *m_WorkerThread;
};

class ManualTrackControl : public QWidget {
  Q_OBJECT

public:
  explicit ManualTrackControl(wgc0310::HeadStatus *headStatus,
                              QWidget *parent = nullptr);

private:
  wgc0310::HeadStatus *m_HeadStatus;
};

#endif
