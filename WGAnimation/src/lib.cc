#include "lib.h"

#include "cwglx/GLImpl.h"

extern "C" {

std::uint32_t WGAPI GetWGAPIVersion(void) {
  // WGAPI 0.1.0
  return 0x00'01'0000;
}

void* WGAPI InitContext(GLFunctions *f) {
  // TODO your codes are here

  Q_UNUSED(f)

  static int g_DummyValueAddr = 0;
  return &g_DummyValueAddr;
}

void WGAPI DestroyContext(void *context, GLFunctions *f) {
  // TODO your codes are here

  Q_UNUSED(context)
  Q_UNUSED(f)
}

bool WGAPI RewindContext(void *context, GLFunctions *f) {
  // TODO your codes are here

  Q_UNUSED(context)
  Q_UNUSED(f)
  return true;
}

bool WGAPI PlayAnimationFrame(void *context,
                              GLFunctions *f,
                              std::uint64_t frame) {
  // TODO your codes are here

  Q_UNUSED(context)
  Q_UNUSED(f)
  Q_UNUSED(frame)
  return true;
}

} // extern "C"
