#ifndef PROJECT_WG_UINEXT_ATTACHMENT_CONTROL_H
#define PROJECT_WG_UINEXT_ATTACHMENT_CONTROL_H

#include "ui_next/CloseSignallingWidget.h"
#include "wgc0310/api/Attachment.h"

namespace wgc0310 {
struct AttachmentStatus;
} // namespace wgc0310

class QMenu;
class QPushButton;
class GLWindow;

class AttachmentControl final : public CloseSignallingWidget {
  Q_OBJECT

public:
  AttachmentControl(wgc0310::AttachmentStatus *attachmentStatus,
                    GLWindow *glWindow);

private slots:
  void ReloadAttachments();

private:
  void LinkButtonAndContextMenu(QPushButton *button,
                                QPushButton *configButton,
                                wgc0310::WGAPIAttachment **attachmentSlot);
  void LinkButtonAndControlWidget(QPushButton *button,
                                  wgc0310::WGAPIAttachment **attachment);

private:
  wgc0310::AttachmentStatus *m_AttachmentStatus;
  GLWindow *m_GLWindow;

  QMenu *m_ItemSelectMenu;
  QPushButton *m_RightBigArmAttachment;
  QPushButton *m_RightSmallArmAttachment;
  QPushButton *m_LeftBigArmAttachment;
  QPushButton *m_LeftSmallArmAttachment;
  QPushButton *m_RightBigArmConfig;
  QPushButton *m_RightSmallArmConfig;
  QPushButton *m_LeftBigArmConfig;
  QPushButton *m_LeftSmallArmConfig;
  QPushButton *m_ReloadButton;

  std::vector<std::unique_ptr<wgc0310::WGAPIAttachment>> m_Attachments;
  std::vector<void*> m_SharedObjects;

  QPushButton *m_ActivatedAttachmentButton;
  QPushButton *m_ActivatedAttachmentConfigButton;
  wgc0310::WGAPIAttachment **m_AttachmentSlot;
};

#endif // PROJECT_WG_UINEXT_ATTACHMENT_CONTROL_H
