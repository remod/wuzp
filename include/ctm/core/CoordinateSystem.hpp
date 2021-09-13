#pragma once

#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"

namespace ctm {

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
    static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate system mismatch.");
    using type = Lhs;
  };

  DEFINE_RECURSIVE_TRAIT(Difference)
  template <typename Lhs, typename Rhs>
  struct Difference<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate system mismatch.");
    using type = Lhs;
  };

  DEFINE_RECURSIVE_TRAIT(Product)
  template <typename Lhs, typename Rhs>
  struct Product<Lhs, Rhs> {
    static_assert(std::is_same<Lhs, Rhs>::value, "Coordinate system mismatch.");
    using type = NoCoordinateSystem;
  };
};

template <int Rows, int Cols, typename... CoordinateSystems>
using CoordinateSystemMatrix = TypeMatrix<Rows, Cols, CoordinateSystemTraits, CoordinateSystems...>;

}  // namespace ctm
