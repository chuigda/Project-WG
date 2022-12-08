#include "ui_next/AttachmentControl.h"

#include <QMenu>
#include <QPushButton>
#include <QFontDatabase>
#include <QBoxLayout>
#include <QDir>
#include <QMessageBox>
#include "wgc0310/AttachmentStatus.h"
#include "ui_next/GLWindow.h"
#include "util/DynLoad.h"

AttachmentControl::AttachmentControl(wgc0310::AttachmentStatus *attachmentStatus,
                                     GLWindow *glWindow)
  : m_AttachmentStatus(attachmentStatus),
    m_GLWindow(glWindow),
    m_ItemSelectMenu(new QMenu()),
    m_RightBigArmAttachment(new QPushButton("右侧大臂")),
    m_RightSmallArmAttachment(new QPushButton("右侧小臂")),
    m_LeftBigArmAttachment(new QPushButton("左侧大臂")),
    m_LeftSmallArmAttachment(new QPushButton("左侧小臂")),
    m_RightBigArmConfig(new QPushButton("⌘")),
    m_RightSmallArmConfig(new QPushButton("⌘")),
    m_LeftBigArmConfig(new QPushButton("⌘")),
    m_LeftSmallArmConfig(new QPushButton("⌘")),
    m_ReloadButton(new QPushButton("↻")),
    m_ActivatedAttachmentButton(nullptr),
    m_ActivatedAttachmentConfigButton(nullptr),
    m_AttachmentSlot(nullptr)
{
  setWindowTitle("附加配件");

  m_RightBigArmConfig->setDisabled(true);
  m_RightSmallArmConfig->setDisabled(true);
  m_LeftBigArmConfig->setDisabled(true);
  m_LeftSmallArmConfig->setDisabled(true);
  m_RightBigArmConfig->setFixedWidth(32);
  m_RightSmallArmConfig->setFixedWidth(32);
  m_LeftBigArmConfig->setFixedWidth(32);
  m_LeftSmallArmConfig->setFixedWidth(32);
  m_ReloadButton->setFixedWidth(32);

  connect(m_GLWindow, &GLWindow::OpenGLInitialized,
          this, &AttachmentControl::ReloadAttachments);
  connect(m_ReloadButton, &QPushButton::clicked,
          this, [this] {
            m_GLWindow->RunWithGLContext([this] {
              ReloadAttachments();
            });
          });

  LinkButtonAndContextMenu(m_RightBigArmAttachment,
                           m_RightBigArmConfig,
                           &m_AttachmentStatus->rightBigArm);
  LinkButtonAndContextMenu(m_RightSmallArmAttachment,
                           m_RightSmallArmConfig,
                           &m_AttachmentStatus->rightSmallArm);
  LinkButtonAndContextMenu(m_LeftBigArmAttachment,
                           m_LeftBigArmConfig,
                           &m_AttachmentStatus->leftBigArm);
  LinkButtonAndContextMenu(m_LeftSmallArmAttachment,
                           m_LeftSmallArmConfig,
                           &m_AttachmentStatus->leftSmallArm);

  LinkButtonAndControlWidget(m_RightBigArmConfig, &m_AttachmentStatus->rightBigArm);
  LinkButtonAndControlWidget(m_RightSmallArmConfig, &m_AttachmentStatus->rightSmallArm);
  LinkButtonAndControlWidget(m_LeftBigArmConfig, &m_AttachmentStatus->leftBigArm);
  LinkButtonAndControlWidget(m_LeftSmallArmConfig, &m_AttachmentStatus->leftSmallArm);

  connect(
    m_ItemSelectMenu,
    &QMenu::triggered,
    this,
    [this] (QAction *triggeredAction) {
      QFont systemFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
      if (triggeredAction->data().isNull()) {
        // 清除配件选择
        if (m_AttachmentSlot) {
          if (*m_AttachmentSlot) {
            QWidget *controlWidget = (*m_AttachmentSlot)->GetControlWidget();
            if (controlWidget) {
              controlWidget->close();
            }
          }
          *m_AttachmentSlot = nullptr;
        }
        if (m_ActivatedAttachmentConfigButton) {
          m_ActivatedAttachmentConfigButton->setDisabled(true);
        }
      } else {
        // 选中一个配件
        systemFont.setBold(true);
        uint attachmentId = triggeredAction->data().toUInt();
        wgc0310::WGAPIAttachment *attachmentPtr = m_Attachments[attachmentId].get();
        if (m_AttachmentSlot) {
          *m_AttachmentSlot = attachmentPtr;
        }

        QWidget *controlWidget = attachmentPtr->GetControlWidget();
        if (m_ActivatedAttachmentConfigButton && controlWidget) {
          m_ActivatedAttachmentConfigButton->setDisabled(false);
        }
      }
      if (m_ActivatedAttachmentButton) {
        m_ActivatedAttachmentButton->setFont(systemFont);
      }
    }
  );

  QHBoxLayout *layout = new QHBoxLayout();
  this->setLayout(layout);

  // Right side
  {
    QVBoxLayout *vBoxRightSide = new QVBoxLayout();
    layout->addLayout(vBoxRightSide);
    {
      QHBoxLayout *h1 = new QHBoxLayout();
      vBoxRightSide->addLayout(h1);

      h1->addWidget(m_RightBigArmConfig);
      h1->addWidget(m_RightBigArmAttachment);
    }

    {
      QHBoxLayout *h2 = new QHBoxLayout();
      vBoxRightSide->addLayout(h2);

      h2->addWidget(m_RightSmallArmConfig);
      h2->addWidget(m_RightSmallArmAttachment);
    }
  }

  // Medium (reload button)
  {
    QVBoxLayout *vBoxCentre = new QVBoxLayout();
    layout->addLayout(vBoxCentre);

    vBoxCentre->addStretch();
    vBoxCentre->addWidget(m_ReloadButton);
    vBoxCentre->addStretch();
  }

  // Left side
  {
    QVBoxLayout *vBoxLeftSide = new QVBoxLayout();
    layout->addLayout(vBoxLeftSide);

    {
      QHBoxLayout *h1 = new QHBoxLayout();
      vBoxLeftSide->addLayout(h1);

      h1->addWidget(m_LeftBigArmAttachment);
      h1->addWidget(m_LeftBigArmConfig);
    }

    {
      QHBoxLayout *h2 = new QHBoxLayout();
      vBoxLeftSide->addLayout(h2);

      h2->addWidget(m_LeftSmallArmAttachment);
      h2->addWidget(m_LeftSmallArmConfig);
    }
  }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "VirtualCallInCtorOrDtor"
  // This should be safe since base class `QWidget` has been initialised,
  // and we did not reimplement `sizeHint`
  setFixedSize(sizeHint());
#pragma clang diagnostic pop
}

void AttachmentControl::ReloadAttachments() {
  m_AttachmentStatus->Reset();
  for (const auto &attachment: m_Attachments) {
    attachment->Delete(m_GLWindow);
  }
  m_Attachments.clear();
  for (auto sharedObject: m_SharedObjects) {
    cw::DetachSharedObject(sharedObject);
  }
  m_SharedObjects.clear();
  m_ItemSelectMenu->clear();
  m_ItemSelectMenu->addAction("移除");
  m_ItemSelectMenu->addSeparator();

  QDir dir(QStringLiteral("attachments"));
  QStringList filters;
  #ifdef CW_WIN32
    filters << "*.dll";
  #else
    filters << "*.so";
  #endif

  QStringList files = dir.entryList(filters, QDir::Files);
  for (const auto &file: files) {
    QString filePath = QStringLiteral("attachments/") + file;
    void *sharedObject = cw::LoadSharedObject(filePath);
    if (!sharedObject) {
      QMessageBox::warning(
        this,
        "附加组件加载错误",
        QString("无法打开共享对象 %1").arg(filePath)
      );
      continue;
    }

    wgc0310::CheckVersionFn checkVersionFn = cw::TryReadSymbol<wgc0310::CheckVersionFn>(
      sharedObject,
      "GetWGAPIVersion"
    );
    if (!checkVersionFn) {
      QMessageBox::warning(
        this,
        "附加组件加载错误",
        QString("无法在共享对象 %1 上定位 <code>GetWGAPIVersion</code>")
          .arg(filePath)
      );
      cw::DetachSharedObject(sharedObject);
      continue;
    }

    if (checkVersionFn() != WGAPI_VERSION) {
      QMessageBox::warning(
        this,
        "附加组件加载错误",
        QString("共享对象 %1 使用的 <code>WGAPI</code> 版本与 Project-WG 程序不匹配")
          .arg(filePath)
      );
      cw::DetachSharedObject(sharedObject);
      continue;
    }

    wgc0310::LoadAttachmentFn loadAttachmentFn = cw::TryReadSymbol<wgc0310::LoadAttachmentFn>(
      sharedObject,
      "LoadAttachment"
    );
    if (!loadAttachmentFn) {
      QMessageBox::warning(
        this,
        "附加组件加载错误",
        QString("无法在共享对象 %1 上定位 <code>LoadAttachment</code>")
          .arg(filePath)
      );
      cw::DetachSharedObject(sharedObject);
      continue;
    }

    wgc0310::WGAPIAttachment *animation = loadAttachmentFn();
    if (!animation) {
      QMessageBox::warning(
        this,
        "附加组件加载错误",
        QString("共享对象 %1 的 <code>LoadAttachment</code> 返回了 <code>nullptr</code>")
          .arg(filePath)
      );
      cw::DetachSharedObject(sharedObject);
      continue;
    }

    animation->Initialize(m_GLWindow);

    QAction *action = m_ItemSelectMenu->addAction(animation->GetName());
    action->setData(QVariant { static_cast<uint>(m_Attachments.size()) });
    m_Attachments.emplace_back(animation);
    m_SharedObjects.push_back(sharedObject);

    QWidget *controlWidget = animation->GetControlWidget();
    if (controlWidget) {
      connect(this, &CloseSignallingWidget::AboutToClose, controlWidget, &QWidget::close);
    }
  }
}

void AttachmentControl::LinkButtonAndContextMenu(QPushButton *button,
                                                 QPushButton *configButton,
                                                 wgc0310::WGAPIAttachment **attachmentSlot)
{
  connect(button, &QPushButton::clicked, this, [this, button, configButton, attachmentSlot] {
    m_AttachmentSlot = attachmentSlot;
    m_ActivatedAttachmentButton = button;
    m_ActivatedAttachmentConfigButton = configButton;
    m_ItemSelectMenu->popup(mapToGlobal(button->pos()));
  });
}

void AttachmentControl::LinkButtonAndControlWidget(QPushButton *button,
                                                   wgc0310::WGAPIAttachment **attachment) {
  connect(button, &QPushButton::clicked, this, [button, attachment] {
    if (!button->isEnabled()) {
      return;
    }

    if (!*attachment) {
      return;
    }

    QWidget *controlWidget = (*attachment)->GetControlWidget();
    if (!controlWidget) {
      return;
    }

    controlWidget->show();
  });
}
