#ifndef PROJECT_WG_FACE_TRACK_CONTROL_H
#define PROJECT_WG_FACE_TRACK_CONTROL_H

#include <QWidget>
#include "ui_next/CloseSignallingWidget.h"

namespace wgc0310 {
struct HeadStatus;
class BodyStatus;

enum class ScreenDisplayMode : std::int8_t;
} // namespace wgc0310

class VTSTrackControl;
class OSFTrackControl;
class ManualTrackControl;

class TrackControl : public CloseSignallingWidget {
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
