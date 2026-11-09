#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/Jacobian.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestJacobian, TransformVelocityVector) {  // NOLINT
  constexpr int FromDimension = 3;
  constexpr int ToDimension = 2;
  using Object =
      Vector<Quantities<ValueMatrix<double, FromDimension, 1>, TypeMatrix<FromDimension, 1, Velocity, Velocity, AngularVelocity>>,
             CoordinateSystemA>;
  using AngleOverLength = Division<Angle, Length>::type;
  using LengthOverAngle = Inverse<AngleOverLength>::type;
  using Transformation = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, FromDimension, ToDimension,
                                  AngleOverLength, AngleOverLength, Dimensionless, Dimensionless, Dimensionless, LengthOverAngle>;
  using TransformedObject =
      Vector<Quantities<ValueMatrix<double, ToDimension, 1>, TypeMatrix<ToDimension, 1, AngularVelocity, Velocity>>, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{14.0, 32.0};
  const Transformation transformation{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_EQ(transformation.transform(object), transformedObject);
}

TEST(TestJacobian, TransformMassMatrix) {  // NOLINT
  constexpr int FromDimension = 3;
  constexpr int ToDimension = 2;
  using Object = Tensor<Indices<Down, Down>,
                        Quantities<ValueMatrix<double, FromDimension, FromDimension>, TypeMatrix<FromDimension, FromDimension, Mass>>,
                        CoordinateSystemA, Relative>;
  using LengthInv = Inverse<Length>::type;
  using Transformation = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, FromDimension, ToDimension,
                                  LengthInv, LengthInv, Dimensionless, Dimensionless, Dimensionless, Length>;
  using MassLength = Product<Mass, Length>::type;
  using MassLength2 = Product<MassLength, Length>::type;
  using TransformedObject = Tensor<Indices<Down, Down>,
                                   Quantities<ValueMatrix<double, ToDimension, ToDimension>,
                                              TypeMatrix<ToDimension, ToDimension, Mass, MassLength, MassLength, MassLength2>>,
                                   CoordinateSystemB, Relative>;
  const Object object{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0, 4.0};
  const Transformation transformation{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  // TODO(remod): Jacobian::inverse() does not exist yet.
  //  EXPECT_EQ(transformation.transform(object), transformedObject);
}

namespace {

template <typename TransformationT_>
concept HasGetTensor = requires(const TransformationT_& transformation) { transformation.getTensor(); };

}  // namespace

// The matrix of a Jacobian between coordinate systems of different dimensions is not a tensor in one coordinate system, whose
// matrix would be square: getTensor() only exists for square matrices, getQuantities() for all.
TEST(TestJacobian, MatrixIsALinearMapOnlyIfSquare) {  // NOLINT
  using NonSquare = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
  using Square = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless>;
  static_assert(!HasGetTensor<NonSquare>);
  static_assert(HasGetTensor<Square>);
  const NonSquare jacobian{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_EQ(jacobian.getQuantities(),
            (Quantities<ValueMatrix<double, 2, 3>, TypeMatrix<2, 3, Dimensionless>>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}));
  const Square square{1.0, 2.0, 3.0, 4.0};
  EXPECT_EQ(square.getTensor(),
            (Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Dimensionless>>, CoordinateSystemA, Relative>{
                1.0, 2.0, 3.0, 4.0}));
}

// The matrices of transformations are compared as quantities, also those of Jacobians between coordinate systems of different
// dimensions, which are not tensors.
TEST(TestJacobian, IsApprox) {  // NOLINT
  using NonSquare = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
  NonSquare jacobian{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};  // Not const: isApprox() is not const yet (#68).
  EXPECT_TRUE(jacobian.isApprox(NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 6.0 + 1e-14}));
  EXPECT_FALSE(jacobian.isApprox(NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 7.0}));
}
