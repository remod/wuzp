#include <gtest/gtest.h>

#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/traits.hpp"

using namespace wuzp;  // NOLINT

TEST(TestCoordinateSystemMatrix, IsSame) {  // NOLINT
  struct CoordinateSystemAName {};
  using CoordinateSystemA = CoordinateSystem<CoordinateSystemAName>;
  using MyCoordinateSystems = TypeMatrix<3, 2, CoordinateSystemTraits, CoordinateSystemA, CoordinateSystemA, CoordinateSystemA,
                                         CoordinateSystemA, CoordinateSystemA, CoordinateSystemA>;
  static_assert(std::is_same<Sum<MyCoordinateSystems, MyCoordinateSystems>::type, MyCoordinateSystems>::value);
}
