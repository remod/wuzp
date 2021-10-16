#pragma once

#include <Eigen/LU>

#include "wuzp/core/traits.hpp"

namespace wuzp {

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
};

}  // namespace wuzp
