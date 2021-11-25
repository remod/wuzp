#include <gtest/gtest.h>

#include "wuzp/core/CoordinateMatrix.hpp"
#include "wuzp/core/CoordinateSystem.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Jacobian.hpp"
#include "wuzp/trafo/LinearTransformation.hpp"

using namespace wuzp;  // NOLINT

#if UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_WUZP
#include "wuzp/units/wuzp/Quantity.hpp"
#include "wuzp/units/wuzp/Unit.hpp"
template <typename Scalar, int Rows, int Cols>
using Quantity = wuzp::Quantity<Scalar, Rows, Cols>;
#elif UNITS_IMPLEMENTATION == UNITS_IMPLEMENTATION_BOOST
#include "wuzp_boost/Quantity.hpp"
#include "wuzp_boost/Unit.hpp"
using namespace wuzp_boost;  // NOLINT
#endif

#if VALUE_MATRIX_IMPLEMENTATION == VALUE_MATRIX_IMPLEMENTATION_EIGEN
#include "wuzp_eigen/ValueMatrix.hpp"
template <typename Scalar, int Rows, int Cols>
using ValueMatrix = wuzp_eigen::ValueMatrix<Scalar, Rows, Cols>;
#endif

TEST(TestJacobian, TransformVelocityVector) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  constexpr int FromDimension = 3;
  constexpr int ToDimension = 2;
  using Object = Vector<CoordinateMatrix<
      QuantityMatrix<ValueMatrix<double, FromDimension, 1>, TypeMatrix<FromDimension, 1, Velocity, Velocity, AngularVelocity>>,
      TypeMatrix<FromDimension, 1, FromCoordinateSystem>>>;
  // Note: With wuzp units angles are dimensionless, but with boost units not.
  using AngleOverLength = Division<Angle, Length>::type;
  using LengthOverAngle = Inverse<AngleOverLength>::type;
  using Transformation =
      Jacobian<ValueMatrix, double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem, FromDimension, ToDimension,
               AngleOverLength, AngleOverLength, Dimensionless, Dimensionless, Dimensionless, LengthOverAngle>;
  using TransformedObject =
      Tensor<1, 0,
             CoordinateMatrix<QuantityMatrix<ValueMatrix<double, ToDimension, 1>, TypeMatrix<ToDimension, 1, AngularVelocity, Velocity>>,
                              TypeMatrix<ToDimension, 1, ToCoordinateSystem>>>;
  const Object object(1.0, 2.0, 3.0);
  const TransformedObject transformedObject(14.0, 32.0);
  const Transformation::ValueMatrixT valueMatrix(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  const Transformation transformation((Transformation::TensorT(valueMatrix)));
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestJacobian, TransformMassMatrix) {  // NOLINT
  struct FromCoordinateSystemName {};
  struct ToCoordinateSystemName {};
  using FromCoordinateSystem = CoordinateSystem<FromCoordinateSystemName>;
  using ToCoordinateSystem = CoordinateSystem<ToCoordinateSystemName>;
  constexpr int FromDimension = 3;
  constexpr int ToDimension = 2;
  using Object = Tensor<
      0, 2,
      CoordinateMatrix<QuantityMatrix<ValueMatrix<double, FromDimension, FromDimension>, TypeMatrix<FromDimension, FromDimension, Mass>>,
                       TypeMatrix<FromDimension, FromDimension, FromCoordinateSystem>>>;
  using LengthInv = Inverse<Length>::type;
  using Transformation = Jacobian<ValueMatrix, double, FromCoordinateSystem, FromCoordinateSystem, ToCoordinateSystem, FromDimension,
                                  ToDimension, LengthInv, LengthInv, Dimensionless, Dimensionless, Dimensionless, Length>;
  using MassLength = Product<Mass, Length>::type;
  using MassLength2 = Product<MassLength, Length>::type;
  using TransformedObject =
      CoordinateMatrix<QuantityMatrix<ValueMatrix<double, ToDimension, ToDimension>,
                                      TypeMatrix<ToDimension, ToDimension, Mass, MassLength, MassLength, MassLength2>>,
                       TypeMatrix<ToDimension, ToDimension, ToCoordinateSystem>>;
  const Object object(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
  const TransformedObject transformedObject(1.0, 2.0, 3.0, 4.0);
  const Transformation transformation(Transformation::TensorT(1.0, 2.0, 3.0, 4.0, 5.0, 6.0));
  // TODO(remod): Jacobian::inverse() does not exist yet.
  //  EXPECT_EQ(transformation.transform(object), transformedObject);
}
