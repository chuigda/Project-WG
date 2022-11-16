#include "ui/Animation.h"
#include "util/Wife.h"

#include <QMessageBox>
#include <QApplication>

#ifdef CW_WIN32

#include <windows.h>

void *LoadSharedObject(const QString& fileName) {
  QString escaped = fileName;
  escaped.replace("/", "\\", Qt::CaseInsensitive);
  HANDLE h = LoadLibraryW(escaped.toStdWString().c_str());
  if (h == nullptr) {
    DWORD err = GetLastError();
    qWarning() << "GetLastError() =" << err;
  }
  return h;
}

template <cw::Wife T>
T TryReadSymbol(void *handle, const char *symbol) {
  return reinterpret_cast<T>(
      GetProcAddress(reinterpret_cast<HMODULE>(handle), symbol)
  );
}

void DetachSharedObject(void *handle) {
  FreeLibrary(reinterpret_cast<HMODULE>(handle));
}

#else

#include <dlfcn.h>

void *LoadSharedObject(const QString& fileName) {
  return dlopen(fileName.toStdString().c_str(), RTLD_LAZY);
}

template <cw::Wife T>
T TryReadSymbol(void *handle, const char *symbol) {
  return reinterpret_cast<T>(dlsym(handle, symbol));
}

void DetachSharedObject(void *handle) {
  dlclose(handle);
}

#endif // CW_WIN32

Animation::Animation(const QString &fileName)
  : m_Handle(LoadSharedObject(fileName)),
    m_InitContextFn(nullptr),
    m_DestroyContextFn(nullptr),
    m_RewindContextFn(nullptr),
    m_PlayAnimationFrameFn(nullptr),
    m_FileName(fileName),
    m_Context(nullptr),
    m_Deleted(true)
{
  if (m_Handle == nullptr) {
    QMessageBox::warning(
        nullptr,
        "警告",
        QString("加载动画文件 %1 失败，该动画将不可用\n")
          .arg(fileName)
    );
    return;
  }

  // Get the WGAPI version
  VersionFn versionFn = TryReadSymbol<VersionFn>(m_Handle, "GetWGAPIVersion");
  if (versionFn == nullptr) {
    QMessageBox::warning(
        nullptr,
        "警告",
        QString("无法在 %1 上定位函数 GetWGAPIVersion，该动画将不可用\n")
          .arg(fileName)
    );
    return;
  }

  std::uint32_t version = versionFn();
  if (version > 0x00'01'00'00) {
    QMessageBox::warning(
        nullptr,
        "致命错误",
        QString("动画文件 %1 报告的 WGAPI 版本与程序不兼容\n"
                "  - 受支持的版本为: 0.1.0\n"
                "  - 文件的版本为: %2.%3.%4\n"
                "该动画将不可用")
          .arg(fileName)
          .arg((version >> 24) & 0xFF)
          .arg((version >> 16) & 0xFF)
          .arg(version & 0xFFFF)
    );
    return;
  }

  AnimationNameFn animationNameFn =
      TryReadSymbol<AnimationNameFn>(m_Handle, "GetAnimationName");
  m_AnimationName = animationNameFn();

  m_InitContextFn =
      TryReadSymbol<InitContextFn>(m_Handle, "InitContext");
  m_DestroyContextFn =
      TryReadSymbol<DestroyContextFn>(m_Handle, "DestroyContext");
  m_RewindContextFn =
      TryReadSymbol<RewindContextFn>(m_Handle, "RewindContext");
  m_PlayAnimationFrameFn =
      TryReadSymbol<PlayAnimationFrameFn>(m_Handle, "PlayAnimationFrame");
  m_Deleted = false;
}

Animation::Animation(Animation &&that) noexcept
  : m_Handle(that.m_Handle),
    m_InitContextFn(that.m_InitContextFn),
    m_DestroyContextFn(that.m_DestroyContextFn),
    m_RewindContextFn(that.m_RewindContextFn),
    m_PlayAnimationFrameFn(that.m_PlayAnimationFrameFn),
    m_FileName(that.m_FileName),
    m_AnimationName(std::move(that.m_AnimationName)),
    m_Context(that.m_Context),
    m_Deleted(that.m_Deleted)
{
  that.m_Deleted = true;
  that.m_Handle = nullptr;
}


Animation::~Animation() {
  if (!m_Deleted) {
    qWarning() << "BodyAnimation::~BodyAnimation() called without calling Delete()";
  }

  if (m_Handle) {
    DetachSharedObject(m_Handle);
  }
}

bool Animation::IsOpen() const noexcept {
  return m_Handle
         && m_InitContextFn
         && m_DestroyContextFn
         && m_RewindContextFn
         && m_PlayAnimationFrameFn;
}

bool Animation::Initialize(GLFunctions *f) {
  if (m_Context != nullptr) {
    qWarning() << "BodyAnimation::Initialize() called twice";
    return true;
  }

  m_Context = m_InitContextFn(f);
  if (m_Context == nullptr) {
    qWarning() << "BodyAnimation::Initialize() failed";
    return false;
  } else {
    return true;
  }
}

bool Animation::StartAnimation(GLFunctions *f) {
  if (m_Context == nullptr) {
    qWarning() << "BodyAnimation::StartAnimation() called before Initialize()";
    return false;
  }

  return m_RewindContextFn(m_Context, f);
}

bool Animation::PlayAnimationFrame(GLFunctions *f, std::uint64_t frame) {
  if (m_Context == nullptr) {
    qWarning() << "BodyAnimation::PlayAnimationFrame() called before Initialize()";
    return false;
  }

  return m_PlayAnimationFrameFn(m_Context, f, frame);
}

void Animation::Delete(GLFunctions *f) {
  if (m_Context != nullptr) {
    m_DestroyContextFn(m_Context, f);
    m_Context = nullptr;
  } else {
    qWarning() << "BodyAnimation::Delete() called twice";
  }

  m_Deleted = true;
}
