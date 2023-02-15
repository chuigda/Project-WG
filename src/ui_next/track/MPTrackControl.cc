#include "TrackControlImpl.h"

#include <QMessageBox>
#include "wgc0310/HeadStatus.h"

void MPTrackControl::HandleError(const QString& error) {
  QMessageBox::warning(this, "MediaPipe 面部/姿态捕捉错误", error);
}

void MPTrackControl::HandleHeadStatus(wgc0310::HeadStatus headStatus) {
  *m_HeadStatus = headStatus;
}
