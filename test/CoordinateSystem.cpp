#include <gtest/gtest.h>

#include "ctm/core/CoordinateSystem.hpp"
#include "ctm/core/TypeMatrix.hpp"
#include "ctm/core/traits.hpp"

using ctm::CoordinateSystem;
using ctm::CoordinateSystemTraits;
using ctm::Sum;
using ctm::TypeMatrix;

TEST(TestCoordinateSystemMatrix, IsSame) {  // NOLINT
  struct CoordinateSystemAName {};
  using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
  using MyCoordinateSystems = TypeMatrix<3, 2, CoordinateSystemTraits, CoordinateSystemA, CoordinateSystemA, CoordinateSystemA,
                                         CoordinateSystemA, CoordinateSystemA, CoordinateSystemA>;
  static_assert(std::is_same<Sum<MyCoordinateSystems, MyCoordinateSystems>::type, MyCoordinateSystems>::value);
}
