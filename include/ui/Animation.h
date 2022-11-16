#ifndef PROJECT_WG_ANIMATION_H
#define PROJECT_WG_ANIMATION_H

#include <cstdint>
#include <QString>

#include "cwglx/GL.h"
#include "cwglx/Texture.h"
#include "util/Derive.h"

struct StaticScreen final {
  QString imageName;
  std::unique_ptr<cw::Texture2D> texture;
};

class Animation final {
public:
  explicit Animation(const QString& fileName);
  ~Animation();

  Animation(Animation const&) = delete;
  Animation& operator=(Animation const&) = delete;

  Animation(Animation&&) = default;
  Animation& operator=(Animation&&) = default;

  [[nodiscard]] bool IsOpen() const noexcept;

  [[nodiscard]] bool Initialize(GLFunctions *f);
  [[nodiscard]] bool StartAnimation(GLFunctions *f);
  [[nodiscard]] bool PlayAnimationFrame(GLFunctions *f, std::uint64_t frame);
  void Delete(GLFunctions *f);

  [[nodiscard]] const QString& GetFileName() const noexcept {
    return m_FileName;
  }

  [[nodiscard]] const QString& GetAnimationName() const noexcept {
    return m_AnimationName;
  }

private:
  using VersionFn = std::uint32_t (*)();
  using AnimationNameFn = const char* (*)();
  using InitContextFn = void* (*)(GLFunctions *f);
  using DestroyContextFn = void (*)(void *context, GLFunctions *f);
  using RewindContextFn = bool (*)(void *context, GLFunctions *f);
  using PlayAnimationFrameFn = bool (*)(void *context,
                                        GLFunctions *f,
                                        std::uint64_t frame);

  void *m_Handle;
  InitContextFn m_InitContextFn;
  DestroyContextFn m_DestroyContextFn;
  RewindContextFn m_RewindContextFn;
  PlayAnimationFrameFn m_PlayAnimationFrameFn;

  QString m_FileName;
  QString m_AnimationName;
  void *m_Context;
  bool m_Deleted;
};

#endif // PROJECT_WG_ANIMATION_H
