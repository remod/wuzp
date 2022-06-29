#pragma once

#include <type_traits>

namespace wuzp_blitz {
namespace internal {

template <typename T0, typename... Ts>
struct CollapseCommas {
  using type = T0;
};
template <typename T0, typename T1, typename... Ts>
struct CollapseCommas<T0, T1, Ts...> {
  using type = decltype(std::declval<T0>(), std::declval<typename CollapseCommas<T1, Ts...>::type>());
};

template <typename Lhs>
Lhs&& spliceCommas(Lhs&& lhs) {
  return std::forward<Lhs>(lhs);
}
template <typename Lhs, typename Rhs, typename... Tail>
typename CollapseCommas<Lhs, Rhs, Tail...>::type&& spliceCommas(Lhs&& lhs, Rhs&& rhs, Tail&&... tail) {
  auto&& firstTwo = (std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  return spliceCommas(std::forward<decltype(firstTwo)>(firstTwo), std::forward<Tail>(tail)...);
}

}  // namespace internal
}  // namespace wuzp_blitz
