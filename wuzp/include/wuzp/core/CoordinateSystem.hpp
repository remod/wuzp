#pragma once

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

namespace wuzp {

template <typename Name>
struct CoordinateSystem {};

// TODO(remod): Use this for now, is there a better solution?f
struct NoCoordinateSystemName {};
using NoCoordinateSystem = CoordinateSystem<NoCoordinateSystemName>;

template <typename Object>
struct GetCoordinateSystem;

template <typename Lhs, typename Rhs>
struct Sum<CoordinateSystem<Lhs>, CoordinateSystem<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
  using type = CoordinateSystem<Lhs>;
};

template <typename Lhs, typename Rhs>
struct Difference<CoordinateSystem<Lhs>, CoordinateSystem<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
  using type = CoordinateSystem<Lhs>;
};

template <typename Lhs, typename Rhs>
struct Product<CoordinateSystem<Lhs>, CoordinateSystem<Rhs>> {
  static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate systems do not match.");
  using type = CoordinateSystem<Lhs>;
};

}  // namespace wuzp
