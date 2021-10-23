#pragma once

#include <utility>

#include <Eigen/LU>

#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename T0, typename... Ts>
struct comma_collapse {
  using type = T0;
};
template <typename T0, typename T1, typename... Ts>
struct comma_collapse<T0, T1, Ts...> {
  using type = decltype(std::declval<T0>(), std::declval<typename comma_collapse<T1, Ts...>::type>());
};

// template <typename LHS>
// LHS&& comma_splice(LHS&& lhs) {
//  return std::forward<LHS>(lhs);
//}
// template <typename LHS, typename RHS, typename... Tail>
// typename comma_collapse<LHS, RHS, Tail...>::type&& comma_splice(LHS&& lhs, RHS&& rhs, Tail&&... tail) {
//  auto&& first_two = (std::forward<LHS>(lhs), std::forward<RHS>(rhs));
//  return comma_splice(std::forward<decltype(first_two)>(first_two), std::forward<Tail>(tail)...);
//}

template <typename LHS>
LHS comma_splice(LHS lhs) {
  return lhs;
}
template <typename LHS, typename RHS, typename... Tail>
typename comma_collapse<LHS, RHS, Tail...>::type comma_splice(LHS lhs, RHS rhs, Tail... tail) {
  auto first_two = (lhs, rhs);
  return comma_splice(first_two, tail...);
}

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

  //  template <typename T, typename... Ts>
  //  struct Construct {
  //    static T value(Ts... ts) {
  //      T t;
  //      t << (..., ts);
  //      return t;
  //    }
  //  };
  template <typename T, typename... Ts>
  struct Construct;
  template <typename Derived, typename T>
  struct Construct<Eigen::EigenBase<Derived>, T> {
    static Derived value(T t) { return Derived(t); }
  };
  //  template <typename T, typename T0, typename... Ts>
  //  struct Construct<T, T0, Ts...> {
  //    static T value(T0&& t0, Ts&&... ts) {
  //      T arr;
  //      comma_splice((arr << std::forward<T0>(t0)), std::forward<Ts>(ts)...);
  //      return arr;
  //    }
  //  };
  template <typename T, typename T0, typename... Ts>
  struct Construct<T, T0, Ts...> {
    static T value(T0 t0, Ts... ts) {
      T arr;
      comma_splice((arr << t0), ts...);
      return arr;
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
