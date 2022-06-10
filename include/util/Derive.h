#ifndef PROJECT_WG_DERIVE_H
#define PROJECT_WG_DERIVE_H

#define CW_DERIVE_UNCOPYABLE(CLASS) \
  CLASS(const CLASS &) = delete;    \
  CLASS &operator=(const CLASS &) = delete;

#define CW_DERIVE_UNMOVABLE(CLASS) \
  CLASS(CLASS &&) = delete;         \
  CLASS &operator=(CLASS &&) = delete;

#endif // PROJECT_WG_DERIVE_H
