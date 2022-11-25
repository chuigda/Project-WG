#ifndef PROJECT_WG_WGC0310_SCREEN_ANIMATION_H
#define PROJECT_WG_WGC0310_SCREEN_ANIMATION_H

#include <cstdint>
#include "cwglx/GL.h"

#ifndef WGAPI
#define WGAPI
#endif // WGAPI

// Current version: 0.2.0
#define WGAPI_VERSION (0x00'02'0000)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  std::uint64_t errorMask;
  const char *description;
  bool isCritical;
  bool ignoreSameError;
} WGAPI_Error;

using WGAPI_InitContextFn = void* WGAPI (*)(GLFunctions *f);
using WGAPI_DestroyContextFn = void WGAPI (*)(void *context, GLFunctions *f);
using WGAPI_RewindContextFn = bool WGAPI (*)(void *context, GLFunctions *f);
using WGAPI_PlayAnimationFrameFn = bool WGAPI (*)(void *context,
                                                  GLFunctions *f,
                                                  std::uint64_t frame);
using WGAPI_SetParamFn = void WGAPI (*)(void *context,
                                        std::size_t paramIdx,
                                        void *value);
using WGAPI_GetErrorFn = WGAPI_Error WGAPI (*)(void *context);

typedef enum {
  WG_PARAM_INT = 0,
  WG_PARAM_REAL = 1,
  WG_PARAM_STRING = 2
} WGAPI_ParamKind;

typedef enum {
  WG_PARAM_DISP_LINE_EDIT,
  WG_PARAM_DISP_TEXT_EDIT,
  WG_PARAM_DISP_SLIDER,
  WG_PARAM_DISP_SPIN_BOX
} WGAPI_ParamDisplay;

typedef struct {
  const char *paramName;
  const char *paramDesc;
  std::uint8_t paramKind;
  std::uint8_t paramDisplay;
} WGAPI_ParamBase;

typedef struct {
  WGAPI_ParamBase paramBase;
  bool hasMin;
  bool hasMax;
  bool hasStep;

  std::int64_t defaultValue;
  std::int64_t min;
  std::int64_t max;
  std::int64_t step;
} WGAPI_IntParam;

typedef struct {
  WGAPI_ParamBase paramBase;
  bool hasMin;
  bool hasMax;
  bool hasStep;

  double defaultValue;
  double min;
  double max;
  double step;
} WGAPI_RealParam;

typedef struct {
  WGAPI_ParamBase paramBase;
  const char *defaultValue;
} WGAPI_StringParam;

typedef struct {
  std::uint32_t version;
  const char *name;

  WGAPI_ParamBase *paramList;
  std::size_t paramCount;

  WGAPI_InitContextFn initContextFn;
  WGAPI_DestroyContextFn destroyContextFn;
  WGAPI_RewindContextFn rewindContextFn;
  WGAPI_PlayAnimationFrameFn playAnimationFrameFn;
  WGAPI_SetParamFn setParamFn;
  WGAPI_GetErrorFn getErrorFn;
} WGAPI_Animation;

using LoadAnimationFn = WGAPI_Animation const* WGAPI (*)();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // PROJECT_WG_WGC0310_SCREEN_ANIMATION_H
