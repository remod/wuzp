#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/JacobianMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestJacobianMatrix, TransformVelocityVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  constexpr int FromDimension = 3;
  constexpr int ToDimension = 2;
  using Object = CoordinateMatrix<ValueMatrix<double, FromDimension, 1>, UnitMatrix<FromDimension, 1, Velocity, Velocity, AngularVelocity>,
                                  CoordinateSystemMatrix<FromDimension, 1, FromCoordinateSystem>>;
  using LengthInv = UnitTraits::Inverse<Length>::type;
  using Transformation = JacobianMatrix<ToDimension, FromDimension, double, FromCoordinateSystem, ToCoordinateSystem, LengthInv, LengthInv,
                                        Dimensionless, Dimensionless, Dimensionless, Length>;
  using TransformedObject = CoordinateMatrix<ValueMatrix<double, ToDimension, 1>, UnitMatrix<ToDimension, 1, AngularVelocity, Velocity>,
                                             CoordinateSystemMatrix<ToDimension, 1, ToCoordinateSystem>>;
  const Object object(Eigen::Vector3d(1.0, 2.0, 3.0));
  const TransformedObject transformedObject(Eigen::Vector2d(14.0, 32.0));
  Transformation::ValueMatrixImpl valueMatrix;
  valueMatrix << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0;
  const Transformation transformation((Transformation::CoordinateMatrixImpl(valueMatrix)));
  EXPECT_EQ(transformation.transform(object), transformedObject);
}
