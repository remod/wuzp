#pragma once

#include <utility>

#include <Eigen/LU>

#include "wuzp/core/traits.hpp"

namespace wuzp {
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

struct ValueMatrixTraits {
  template <typename T>
  struct Scalar {
    using type = typename std::decay_t<T>::Scalar;
  };

  template <typename T>
  struct Rows {
    static constexpr int value = std::decay_t<T>::RowsAtCompileTime;
  };

  template <typename T>
  struct Cols {
    static constexpr int value = std::decay_t<T>::ColsAtCompileTime;
  };

  template <typename T>
  struct Dimension {
    static constexpr int value = static_cast<int>(Rows<T>::value > 1) + static_cast<int>(Cols<T>::value > 1);
  };

  template <typename T, typename... Ts>
  struct Construct;
  template <typename T>
  struct Construct<T> {
    static T value() { return T(); }
  };
  template <typename T, typename T0>
  struct Construct<T, T0> {
    static T value(T0 t0) { return T(t0); }
  };
  template <typename T, typename T0, typename... Ts>
  struct Construct<T, T0, Ts...> {
    static T value(T0 t0, Ts... ts) {
      T t;
      internal::spliceCommas((t << std::forward<T0>(t0)), std::forward<Ts>(ts)...);
      return t;
    }
  };

  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    // Use decltype/declval to allow for lazy evaluation.
    // TODO(remod): Use std::decay to avoid the introduction of const?
    using type = decltype(std::declval<const Lhs>() + std::declval<const Rhs>());
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Rhs> {
    // Use decltype/declval to allow for lazy evaluation.
    using type = decltype(std::declval<const Lhs>() - std::declval<const Rhs>());
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename Lhs, typename Rhs>
  struct Product<Lhs, Rhs> {
    // Use decltype/declval to allow for lazy evaluation.
    using type = decltype(std::declval<const Lhs>() * std::declval<const Rhs>());
  };

  DEFINE_RECURSIVE_TRAIT(Division)
  template <typename Lhs, typename Rhs>
  struct Division<Lhs, Rhs> {
    // Use decltype/declval to allow for lazy evaluation.
    using type = decltype(std::declval<const Lhs>() / std::declval<const Rhs>());
  };

  template <typename T>
  struct Transpose {
    // Use decltype/declval to allow for lazy evaluation.
    using type = decltype(std::declval<const T>().transpose());
  };

  template <typename Lhs, typename Rhs>
  struct DotProduct {
    // Use decltype/declval to allow for lazy evaluation.
    using type = decltype(std::declval<const Lhs>().dot(std::declval<const Rhs>()));
  };

  template <typename T>
  struct Inverse {
    // Use decltype/declval to allow for lazy evaluation.
    using type = decltype(std::declval<const T>().inverse());
  };

  template <typename T, int BlockRowId, int BlockColId, int BlockRows, int BlockCols>
  struct Block {
    // Use decltype/declval to allow for lazy evaluation.
    using type = decltype(std::declval<const T>().template block<BlockRows, BlockCols>(BlockRowId, BlockColId));
  };

  template <typename T>
  struct Zero {
    static auto value() { return T::Zero(); }
  };

  template <typename T>
  struct Evaluate {
    using type = std::decay_t<decltype(std::declval<T>().eval())>;
  };
  template <typename T>
  static auto evaluate(T t) {
    return typename Evaluate<T>::type(t);
  }
};

}  // namespace wuzp
