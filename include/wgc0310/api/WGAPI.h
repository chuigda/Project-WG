#ifndef PROJECT_WG_WGC0310_WGAPI_H
#define PROJECT_WG_WGC0310_WGAPI_H

#include <cstdint>

#ifndef WGAPI
#define WGAPI
#endif // WGAPI

// Current version: 0.4.0
#define WGAPI_VERSION (0x00'04'0000)

namespace wgc0310 {
extern "C" {
using CheckVersionFn = std::uint32_t WGAPI (*)();
} // extern "C"
} // namespace wgc0310

#endif // PROJECT_WG_WGC0310_WGAPI_H
