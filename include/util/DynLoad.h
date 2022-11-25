#ifndef PROJECT_WG_DYN_LOAD_H
#define PROJECT_WG_DYN_LOAD_H

#include "util/Wife.h"

class QString;

namespace cw {

void *LoadSharedObject(QString const& fileName);
void *TryReadSymbolImpl(void *handle, const char *funcName);
void DetachSharedObject(void *handle);

template <Wife T>
T LoadSymbol(void *handle, const char *funcName) {
  return reinterpret_cast<T>(TryReadSymbolImpl(handle, funcName));
}

} // namespace cw

#endif // PROJECT_WG_DYN_LOAD_H
