#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename Name>
struct CoordinateSystem {};

// TODO(remod): Use this for now, is there a better solution?
struct NoCoordinateSystemName {};
using NoCoordinateSystem = CoordinateSystem<NoCoordinateSystemName>;

template <typename Object>
struct GetCoordinateSystem;

struct CoordinateSystemTraits {
  DEFINE_RECURSIVE_TRAIT(Sum)
  template <typename Lhs, typename Rhs>
  struct Sum<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
    using type = Lhs;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
    using type = Lhs;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename Lhs, typename Rhs>
  struct Product<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
    using type = NoCoordinateSystem;
  };
};

}  // namespace wuzp
