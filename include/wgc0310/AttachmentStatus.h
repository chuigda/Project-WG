#ifndef PROJECT_WG_ATTACHMENT_STATUS_H
#define PROJECT_WG_ATTACHMENT_STATUS_H

#include "wgc0310/api/Attachment.h"
#include "util/Derive.h"

namespace wgc0310 {

struct AttachmentStatus {
  WGAPIAttachment *rightBigArm = nullptr;
  WGAPIAttachment *rightSmallArm = nullptr;

  WGAPIAttachment *leftBigArm = nullptr;
  WGAPIAttachment *leftSmallArm = nullptr;

  AttachmentStatus() = default;

  constexpr inline void Reset() noexcept {
    rightBigArm = nullptr;
    rightSmallArm = nullptr;
    leftBigArm = nullptr;
    leftSmallArm = nullptr;
  }

  CW_DERIVE_UNCOPYABLE(AttachmentStatus)
  CW_DERIVE_UNMOVABLE(AttachmentStatus)
};

} // namespace wgc0310

#endif // PROJECT_WG_ATTACHMENT_STATUS_H
