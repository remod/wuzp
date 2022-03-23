#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Jacobian.hpp"

#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp_units/Unit.hpp"
using namespace wuzp_units;  // NOLINT
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUES_IMPLEMENTATION == VALUES_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;

TEST(TestJacobian, TransformVelocityVector) {  // NOLINT
  constexpr int FromDimension = 3;
  constexpr int ToDimension = 2;
  using Object =
      Vector<Quantities<ValueMatrix<double, FromDimension, 1>, TypeMatrix<FromDimension, 1, Velocity, Velocity, AngularVelocity>>,
             CoordinateSystemA>;
  // Note: With wuzp units angles are dimensionless, but with boost units not.
  using AngleOverLength = Division<Angle, Length>::type;
  using LengthOverAngle = Inverse<AngleOverLength>::type;
  using Transformation = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, FromDimension, ToDimension,
                                  AngleOverLength, AngleOverLength, Dimensionless, Dimensionless, Dimensionless, LengthOverAngle>;
  using TransformedObject =
      Tensor<1, 0, Quantities<ValueMatrix<double, ToDimension, 1>, TypeMatrix<ToDimension, 1, AngularVelocity, Velocity>>,
             CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(14.0, 32.0);
  const Transformation transformation(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestJacobian, TransformMassMatrix) {  // NOLINT
  constexpr int FromDimension = 3;
  constexpr int ToDimension = 2;
  using Object = Tensor<0, 2, Quantities<ValueMatrix<double, FromDimension, FromDimension>, TypeMatrix<FromDimension, FromDimension, Mass>>,
                        CoordinateSystemA>;
  using LengthInv = Inverse<Length>::type;
  using Transformation = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, FromDimension, ToDimension,
                                  LengthInv, LengthInv, Dimensionless, Dimensionless, Dimensionless, Length>;
  using MassLength = Product<Mass, Length>::type;
  using MassLength2 = Product<MassLength, Length>::type;
  using TransformedObject = Tensor<0, 2,
                                   Quantities<ValueMatrix<double, ToDimension, ToDimension>,
                                              TypeMatrix<ToDimension, ToDimension, Mass, MassLength, MassLength, MassLength2>>,
                                   CoordinateSystemB>;
  const Object object(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0, 4.0);
  const Transformation transformation(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  // TODO(remod): Jacobian::inverse() does not exist yet.
  //  EXPECT_EQ(transformation.transform(object), transformedObject);
}

#endif
