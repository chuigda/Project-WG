#ifndef PROJECT_WG_WIFE_H
#define PROJECT_WG_WIFE_H

namespace cw {

/// 这个 Concept 没有任何实质性作用，但它表明即使是 C++ 类型也应该是我的老婆
template <typename T>
concept Wife = true;

} // namespace cw

#endif // PROJECT_WG_WIFE_H
