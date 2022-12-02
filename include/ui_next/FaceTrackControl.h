#ifndef PROJECT_WG_FACE_TRACK_CONTROL_H
#define PROJECT_WG_FACE_TRACK_CONTROL_H

#include <QWidget>

namespace wgc0310 {
struct HeadStatus;
} // namespace wgc0310

class VTSTrackControl;
class OSFTrackControl;
class ManualTrackControl;

class TrackControl : public QWidget {
  Q_OBJECT

public:
  TrackControl(wgc0310::HeadStatus *headStatus, QThread *workerThread);

private:
  // Output
  wgc0310::HeadStatus *m_HeadStatus;

  // Control widgets
  VTSTrackControl *m_VTSTrackControl;
  OSFTrackControl *m_OSFTrackControl;
  ManualTrackControl *m_ManualTrackControl;

  // Shared worker thread
  QThread *m_WorkerThread;
};

#endif // PROJECT_WG_FACE_TRACK_CONTROL_H
