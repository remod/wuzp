#include <gtest/gtest.h>

#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/core/Units.hpp"

#include "Unit.hpp"

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

TEST(TestUnitMatrix, InnerProduct) {  // NOLINT
  static_assert(std::is_same<InnerProduct<TypeMatrix<2, 2, Length>, TypeMatrix<2, 2, Length>>::type, TypeMatrix<2, 2, Area>>::value);
}

// The units of the matrix inverse are the transposed reciprocals: (M^-1)_ij = b_i / a_j for M_ij = a_i / b_j (Hart, Multidimensional
// Analysis, 1995).
TEST(TestUnitMatrix, Inverse) {  // NOLINT
  static_assert(std::is_same<Inverse<TypeMatrix<2, 2, Time>>::type, TypeMatrix<2, 2, Frequency>>::value);
  // A linear map from (mass, dimensionless) to (length, time): a = (m, s), b = (kg, 1).
  using LinearMap = TypeMatrix<2, 2, Division<Length, Mass>::type, Length, Division<Time, Mass>::type, Time>;
  using LinearMapInverse =
      TypeMatrix<2, 2, Division<Mass, Length>::type, Division<Mass, Time>::type, Inverse<Length>::type, Inverse<Time>::type>;
  static_assert(std::is_same<Inverse<LinearMap>::type, LinearMapInverse>::value);
  static_assert(std::is_same<Inverse<LinearMapInverse>::type, LinearMap>::value);
  static_assert(std::is_same<Inverse<TypeMatrix<1, 1, Time>>::type, TypeMatrix<1, 1, Frequency>>::value);
}

// The units of a matrix describe a linear map if they are M_ij = a_i / b_j, i.e. if M_ij M_00 = M_i0 M_0j.
TEST(TestUnitMatrix, UnitsDescribeLinearMap) {  // NOLINT
  static_assert(UnitsDescribeLinearMap<TypeMatrix<1, 1, Time>>::value);
  static_assert(UnitsDescribeLinearMap<TypeMatrix<3, 3, Length>>::value);
  using MassLength = Product<Mass, Length>::type;
  using MassArea = Product<Mass, Area>::type;
  static_assert(UnitsDescribeLinearMap<TypeMatrix<2, 2, Mass, MassLength, MassLength, MassArea>>::value);
  static_assert(UnitsDescribeLinearMap<TypeMatrix<2, 2, Division<Length, Mass>::type, Length, Division<Time, Mass>::type, Time>>::value);
  // A matrix that is not square can describe a linear map between spaces of different dimensions, e.g. a Jacobian.
  static_assert(UnitsDescribeLinearMap<TypeMatrix<2, 3, Length, Area, Volume, Dimensionless, Length, Area>>::value);
  static_assert(!UnitsDescribeLinearMap<TypeMatrix<2, 2, Length, Length, Length, Time>>::value);
  static_assert(!UnitsDescribeLinearMap<TypeMatrix<2, 2, Mass, MassLength, MassLength, Mass>>::value);
  // Every element is checked, not only the diagonal or the last one: in both matrices, only M_12 M_00 != M_10 M_02.
  static_assert(!UnitsDescribeLinearMap<TypeMatrix<3, 3, Length, Length, Length, Length, Length, Time, Length, Length, Length>>::value);
  static_assert(!UnitsDescribeLinearMap<TypeMatrix<2, 3, Length, Area, Volume, Dimensionless, Length, Length>>::value);
}
