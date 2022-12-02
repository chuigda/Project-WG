#include "TrackControlImpl.h"

ManualTrackControl::ManualTrackControl(wgc0310::HeadStatus *headStatus,
                                       QWidget *parent)
  : QWidget(parent),
    m_HeadStatus(headStatus)
{}
