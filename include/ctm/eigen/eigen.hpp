#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "ctm/core/traits.hpp"

namespace ctm {

struct ValuesTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    // Use decltype/declval to allow for lazy evaluation.
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
};

}  // namespace ctm
