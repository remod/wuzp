#include <gtest/gtest.h>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

using namespace wuzp;  // NOLINT

TEST(TestUnitMatrix, Sum) {  // NOLINT
  static_assert(std::is_same<Sum<TypeMatrix<2, 2, Length>, TypeMatrix<2, 2, Length>>::type, TypeMatrix<2, 2, Length>>::value);
  using MyUnitMatrix = TypeMatrix<3, 2, Length, Area, Volume, Mass, Force, Energy>;
  static_assert(std::is_same<Sum<MyUnitMatrix, MyUnitMatrix>::type, MyUnitMatrix>::value);
}

TEST(TestUnitMatrix, Difference) {  // NOLINT
  static_assert(std::is_same<Difference<TypeMatrix<2, 2, Length>, TypeMatrix<2, 2, Length>>::type, TypeMatrix<2, 2, Length>>::value);
  using MyUnitMatrix = TypeMatrix<3, 2, Length, Area, Volume, Mass, Force, Energy>;
  static_assert(std::is_same<Difference<MyUnitMatrix, MyUnitMatrix>::type, MyUnitMatrix>::value);
}

TEST(TestUnitMatrix, Product) {  // NOLINT
  static_assert(std::is_same<Product<TypeMatrix<2, 2, Length>, TypeMatrix<2, 2, Length>>::type, TypeMatrix<2, 2, Area>>::value);
}

TEST(TestUnitMatrix, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<TypeMatrix<2, 2, Time>>::type, TypeMatrix<2, 2, Frequency>>::value);
}
