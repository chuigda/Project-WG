#ifndef PROJECT_WG_FACE_TRACK_CONTROL_H
#define PROJECT_WG_FACE_TRACK_CONTROL_H

#include <QWidget>
#include "wgc0310/HeadStatus.h"
#include "ui_next/CloseSignallingWidget.h"

class VTSTrackControl;
class OSFTrackControl;
class ManualTrackControl;

class TrackControl : public CloseSignallingWidget {
  Q_OBJECT

public:
  TrackControl(wgc0310::HeadStatus *headStatus,
               wgc0310::ScreenDisplayMode *screenDisplayMode,
               QThread *workerThread);

private:
  // Output
  wgc0310::HeadStatus *m_HeadStatus;
  wgc0310::ScreenDisplayMode *m_ScreenDisplayMode;

  // Control widgets
  VTSTrackControl *m_VTSTrackControl;
  OSFTrackControl *m_OSFTrackControl;
  ManualTrackControl *m_ManualTrackControl;

  // Shared worker thread
  QThread *m_WorkerThread;
};

#endif // PROJECT_WG_FACE_TRACK_CONTROL_H
