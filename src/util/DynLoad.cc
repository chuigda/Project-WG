#include "util/DynLoad.h"

#include <QString>
#include <QDebug>

#ifdef CW_WIN32
#include <windows.h>
#endif // CW_WIN32

namespace cw {

#ifdef CW_WIN32

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

void* TryReadSymbolImpl(void *handle, const char *symbol) {
  return reinterpret_cast<void*>(
      GetProcAddress(reinterpret_cast<HMODULE>(handle), symbol)
  );
}

void DetachSharedObject(void *handle) {
  FreeLibrary(reinterpret_cast<HMODULE>(handle));
}

#else

#include <dlfcn.h>

void *LoadSharedObject(const QString& fileName) {
  void *descriptor = dlopen(fileName.toStdString().c_str(), RTLD_LAZY);
  if (descriptor == nullptr) {
    qWarning() << "dlError() =" << dlerror();
  }
  return descriptor;
}

void *TryReadSymbolImpl(void *handle, const char *symbol) {
  return reinterpret_cast<void*>(dlsym(handle, symbol));
}

void DetachSharedObject(void *handle) {
  dlclose(handle);
}

#endif // CW_WIN32

} // namespace cw
