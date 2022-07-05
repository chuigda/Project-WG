#ifndef PROJECT_WG_WGAPI_LIBANIM_H
#define PROJECT_WG_WGAPI_LIBANIM_H

#define WGAPI

#include <cstdint>

#include "cwglx/GL.h"

extern "C" {

std::uint32_t WGAPI GetWGAPIVersion();
const char* WGAPI GetAnimationName();

void* WGAPI InitContext(GLFunctions *f);
void WGAPI DestroyContext(void *context, GLFunctions *f);

bool WGAPI RewindContext(void *context, GLFunctions *f);
bool WGAPI PlayAnimationFrame(void *context,
                              GLFunctions *f,
                              std::uint64_t frame);

} // extern "C"

#endif // PROJECT_WG_WGAPI_LIBANIM_H
