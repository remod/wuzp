#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename Name>
struct CoordinateSystemTemplate {};

// TODO(remod): Use this for now, is there a better solution?
struct NoCoordinateSystemName {};
using NoCoordinateSystem = CoordinateSystemTemplate<NoCoordinateSystemName>;

template <typename Lhs, typename Rhs>
struct Sum<CoordinateSystemTemplate<Lhs>, CoordinateSystemTemplate<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
  using type = CoordinateSystemTemplate<Lhs>;
};

template <typename Lhs, typename Rhs>
struct Difference<CoordinateSystemTemplate<Lhs>, CoordinateSystemTemplate<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
  using type = CoordinateSystemTemplate<Lhs>;
};

template <typename Lhs, typename Rhs>
struct Product<CoordinateSystemTemplate<Lhs>, CoordinateSystemTemplate<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
  using type = CoordinateSystemTemplate<Lhs>;
};

}  // namespace wuzp
