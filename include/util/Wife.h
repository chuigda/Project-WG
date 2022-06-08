#ifndef PRWG_WIFE_H
#define PRWG_WIFE_H

namespace cw {

/// 这个 Concept 没有任何实质性作用，但它表明所有类型都应该是我的老婆
template <typename T>
concept Wife = true;

} // namespace cw

#endif // PRWG_WIFE_H
