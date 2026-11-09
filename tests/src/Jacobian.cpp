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
  using Transformation = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, FromDimension, ToDimension, AngleOverLength,
                                  AngleOverLength, Dimensionless, Dimensionless, Dimensionless, LengthOverAngle>;
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
  using Transformation = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, FromDimension, ToDimension, LengthInv,
                                  LengthInv, Dimensionless, Dimensionless, Dimensionless, Length>;
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

// The matrix of a Jacobian, like that of every passive linear transformation, maps one coordinate system into another, possibly
// of another dimension: it is not a tensor in one coordinate system, also if it is square, so there is no getTensor(), only
// getQuantities().
TEST(TestJacobian, MatrixIsNotATensor) {  // NOLINT
  using NonSquare = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
  using Square = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless>;
  static_assert(!HasGetTensor<NonSquare>);
  static_assert(!HasGetTensor<Square>);
  const NonSquare jacobian{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_EQ(jacobian.getQuantities(),
            (Quantities<ValueMatrix<double, 2, 3>, TypeMatrix<2, 3, Dimensionless>>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}));
  const Square square{1.0, 2.0, 3.0, 4.0};
  EXPECT_EQ(square.getQuantities(), (Quantities<ValueMatrix<double, 2, 2>, TypeMatrix<2, 2, Dimensionless>>{1.0, 2.0, 3.0, 4.0}));
}

// The matrices of transformations are compared as quantities, also those of Jacobians between coordinate systems of different
// dimensions, which are not tensors. isApprox is const and takes a precision, like that of tensors (#68).
TEST(TestJacobian, IsApprox) {  // NOLINT
  using NonSquare = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;
  const NonSquare jacobian{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_TRUE(jacobian.isApprox(NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 6.0 + 1e-14}));
  EXPECT_FALSE(jacobian.isApprox(NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 7.0}));
  EXPECT_TRUE(jacobian.isApprox(NonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 6.001}, 1e-3));
}

namespace {

// The Jacobian of the Cartesian coordinates (x, y) in B with respect to the polar coordinates (r, phi) in A, where
// x = r cos(phi) and y = r sin(phi), at r = 2 m and phi = pi/2: (cos(phi), -r sin(phi); sin(phi), r cos(phi)).
using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless, Length, Dimensionless, Length>;
using VelocityInA = Vector<Quantities<ValueMatrix<double, 2, 1>, TypeMatrix<2, 1, Velocity, AngularVelocity>>, CoordinateSystemA>;
const JacobianBA jacobianBA{0.0, -2.0, 1.0, 0.0};

}  // namespace

// The composition of Jacobians is the chain rule, J_CA = J_CB J_BA, e.g. of polar coordinates (r, phi) in A, Cartesian coordinates
// (x, y) in B, and the coordinates (x + y, x - y) in C.
TEST(TestJacobian, ChainRule) {  // NOLINT
  using JacobianCB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 2, 2, Dimensionless>;
  using JacobianCA =
      Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 2, 2, Dimensionless, Length, Dimensionless, Length>;
  using VelocityInC = VelocityVector2<ValueMatrix, double, CoordinateSystemC>;
  const JacobianCB jacobianCB{1.0, 1.0, 1.0, -1.0};
  const JacobianCA jacobianCA = jacobianCB * jacobianBA;
  EXPECT_EQ(jacobianCA, (JacobianCA{1.0, -2.0, -1.0, -2.0}));
  const VelocityInA velocity{1.0, 0.5};
  EXPECT_EQ(jacobianCA.transform(velocity), jacobianCB.transform(jacobianBA.transform(velocity)));
  EXPECT_EQ(jacobianCA.transform(velocity), (VelocityInC{0.0, -2.0}));
}

// The inverse of a Jacobian is the Jacobian of the inverse coordinate change, J_AB = J_BA^-1, with the inverse units of the
// transposed matrix: that of the polar coordinates with respect to the Cartesian ones, (cos(phi), sin(phi); -sin(phi) / r,
// cos(phi) / r). It is the matrix inverse, which Blitz++ does not provide.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestJacobian, Inverse) {  // NOLINT
  using LengthInv = Inverse<Length>::type;
  using JacobianAB =
      Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA, 2, 2, Dimensionless, Dimensionless, LengthInv, LengthInv>;
  using JacobianAA =
      Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, 2, 2, Dimensionless, Length, LengthInv, Dimensionless>;
  using JacobianBB = Jacobian<ValueMatrix, double, CoordinateSystemB, CoordinateSystemB, 2, 2, Dimensionless>;
  const JacobianAB jacobianAB = jacobianBA.inverse();
  EXPECT_TRUE(jacobianAB.isApprox(JacobianAB{0.0, 1.0, -0.5, 0.0}, 1e-12));
  EXPECT_TRUE((jacobianAB * jacobianBA).isApprox(JacobianAA{1.0, 0.0, 0.0, 1.0}, 1e-12));
  EXPECT_TRUE((jacobianBA * jacobianAB).isApprox(JacobianBB{1.0, 0.0, 0.0, 1.0}, 1e-12));
  const VelocityInA velocity{1.0, 0.5};
  EXPECT_TRUE(jacobianAB.transform(jacobianBA.transform(velocity)).isApprox(velocity, 1e-12));
  const VelocityVector2<ValueMatrix, double, CoordinateSystemB> velocityInB{1.0, 2.0};
  EXPECT_TRUE(jacobianBA.transform(jacobianAB.transform(velocityInB)).isApprox(velocityInB, 1e-12));
}
#endif
