#include "TrackControlImpl.h"

VTSTrackControl::VTSTrackControl(wgc0310::HeadStatus *headStatus,
                                 QThread *workerThread,
                                 QWidget *parent)
  : QWidget(parent),
    m_HeadStatus(headStatus),
    m_WorkerThread(workerThread)
{}
