#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

// A general linear scaling, which is not orthogonal: A^-1 != A^T.
using Scaling = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, Dimensionless>;

template <typename CoordinateSystemT>
using ForceCovector = HomogeneousCovector<3, ValueMatrix, double, Force, CoordinateSystemT>;
template <typename CoordinateSystemT>
using Covariance = Tensor<Indices<Up, Up>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Area>>, CoordinateSystemT, Relative>;
template <typename CoordinateSystemT>
using VelocityGradient =
    Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Frequency>>, CoordinateSystemT, Relative>;
template <typename CoordinateSystemT>
using MassMatrix = Tensor<Indices<Down, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Mass>>, CoordinateSystemT, Relative>;

const Scaling scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0};  // Scaling by 2, 4 and 8.

TEST(TestPassiveLinearTransformation, StreamOperator) {  // NOLINT
  EXPECT_EQ(toString(scaling), toString(scaling.getQuantities()));
}

// Covectors and (1,1) tensors transform with the inverse.
TEST(TestPassiveLinearTransformation, TransformCovector) {  // NOLINT
  // A covector transforms with the inverse: w' = w A^-1.
  const ForceCovector<CoordinateSystemA> object{8.0, 8.0, 8.0};
  const ForceCovector<CoordinateSystemB> transformedObject{4.0, 2.0, 1.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject, 1e-12));
}

TEST(TestPassiveLinearTransformation, TransformCovectorAndVectorPreservesTheirPairing) {  // NOLINT
  // The work done by a force along a displacement does not depend on the coordinates: w' v' = w A^-1 A v = w v.
  const ForceCovector<CoordinateSystemA> force{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> displacement{4.0, 5.0, 6.0};
  const QuantityD<Energy> work{innerProduct(force, displacement).getQuantity()};
  const QuantityD<Energy> transformedWork{innerProduct(scaling.transform(force), scaling.transform(displacement)).getQuantity()};
  EXPECT_TRUE(transformedWork.isApprox(work, 1e-12));
  EXPECT_TRUE(work.isApprox(QuantityD<Energy>{32.0}, 1e-12));
}

TEST(TestPassiveLinearTransformation, TransformTensorWithOneContravariantAndOneCovariantIndex) {  // NOLINT
  // A linear map, e.g. a velocity gradient, transforms like T' = A T A^-1, i.e. T'_ij = a_i / a_j T_ij for a scaling A = diag(a).
  const VelocityGradient<CoordinateSystemA> object{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const VelocityGradient<CoordinateSystemB> transformedObject{1.0, 0.5, 0.25, 2.0, 1.0, 0.5, 4.0, 2.0, 1.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject, 1e-12));
}

TEST(TestPassiveLinearTransformation, TransformTensorWithTwoContravariantIndices) {  // NOLINT
  // A covariance transforms like T' = A T A^T, i.e. T'_ij = a_i a_j T_ij for a scaling A = diag(a).
  const Covariance<CoordinateSystemA> object{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const Covariance<CoordinateSystemB> transformedObject{4.0, 8.0, 16.0, 8.0, 16.0, 32.0, 16.0, 32.0, 64.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject, 1e-12));
}

TEST(TestPassiveLinearTransformation, TransformOuterProductOfVectors) {  // NOLINT
  // The outer product of a vector with itself transforms like the outer product of the transformed vectors.
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const Covariance<CoordinateSystemA> object{outerProduct(vector, vector)};
  const auto transformedVector = scaling.transform(vector);
  const Covariance<CoordinateSystemB> transformedObject{outerProduct(transformedVector, transformedVector)};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject, 1e-12));
}

// A transformation can be copied from a non-const object: the forwarding constructor does not hijack the copy (#66).
TEST(TestPassiveLinearTransformation, CopyFromNonConstObject) {  // NOLINT
  Scaling transformation{scaling};
  const Scaling copy(transformation);
  EXPECT_EQ(copy, scaling);
}

// isApprox is const, takes a precision, and checks From and To (#68).
TEST(TestPassiveLinearTransformation, IsApprox) {  // NOLINT
  EXPECT_TRUE(scaling.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0 + 1e-14}));
  EXPECT_FALSE(scaling.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 9.0}));
  EXPECT_TRUE(scaling.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.001}, 1e-3));
  EXPECT_FALSE(scaling.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.001}, 1e-6));
}

// Composition in the order of Drake's monogram notation, X_CB * X_BA = X_CA: the product of the matrices.
TEST(TestPassiveLinearTransformation, Compose) {  // NOLINT
  using ScalingCB = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 3, Dimensionless>;
  using ScalingCA = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 3, Dimensionless>;
  const ScalingCB scalingCB{0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const ScalingCA scalingCA = scalingCB * scaling;
  EXPECT_EQ(scalingCA, (ScalingCA{0.0, 4.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 8.0}));
  EXPECT_EQ(scalingCA.transform(vector), scalingCB.transform(scaling.transform(vector)));
}

namespace {

using SwapCB = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, 3, Dimensionless>;
using ScalingCA = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC, 3, Dimensionless>;
const SwapCB swapCB{0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};  // Swaps x and y.

}  // namespace

// The composition transforms every tensor like applying the right-hand side first, then the left-hand side: here the scaling
// diag(2, 4, 8) from A to B, then swapping x and y from B to C, i.e. the matrix A = ((0, 4, 0), (2, 0, 0), (0, 0, 8)) from A to C.
// v' = A v, and T' = A T A^T.
TEST(TestPassiveLinearTransformation, ComposeTransformsPointsVectorsAndTensors) {  // NOLINT
  const ScalingCA composition = swapCB * scaling;
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  EXPECT_EQ(composition.transform(point), swapCB.transform(scaling.transform(point)));
  EXPECT_EQ(composition.transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemC>{8.0, 2.0, 24.0}));
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  EXPECT_EQ(composition.transform(vector), swapCB.transform(scaling.transform(vector)));
  EXPECT_EQ(composition.transform(vector), (PositionVector3<ValueMatrix, double, CoordinateSystemC>{8.0, 2.0, 24.0}));
  const Covariance<CoordinateSystemA> covariance{1.0, 1.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 3.0};
  EXPECT_EQ(composition.transform(covariance), swapCB.transform(scaling.transform(covariance)));
  EXPECT_EQ(composition.transform(covariance), (Covariance<CoordinateSystemC>{32.0, 8.0, 0.0, 8.0, 4.0, 0.0, 0.0, 0.0, 192.0}));
}

// Covectors and tensors with lower indices transform with the inverse. The inverse of the composition is ((0, 1/2, 0), (1/4, 0, 0), (0, 0,
// 1/8)).
TEST(TestPassiveLinearTransformation, ComposeTransformsCovectorsAndTensorsWithLowerIndices) {  // NOLINT
  const ScalingCA composition = swapCB * scaling;
  // w' = w A^-1.
  const ForceCovector<CoordinateSystemA> covector{8.0, 8.0, 8.0};
  EXPECT_TRUE(composition.transform(covector).isApprox(swapCB.transform(scaling.transform(covector)), 1e-12));
  EXPECT_TRUE(composition.transform(covector).isApprox(ForceCovector<CoordinateSystemC>{2.0, 4.0, 1.0}, 1e-12));
  // T' = A T A^-1.
  const VelocityGradient<CoordinateSystemA> velocityGradient{1.0, 1.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 3.0};
  EXPECT_TRUE(composition.transform(velocityGradient).isApprox(swapCB.transform(scaling.transform(velocityGradient)), 1e-12));
  EXPECT_TRUE(composition.transform(velocityGradient)
                  .isApprox(VelocityGradient<CoordinateSystemC>{2.0, 2.0, 0.0, 0.5, 1.0, 0.0, 0.0, 0.0, 3.0}, 1e-12));
  // M' = A^-T M A^-1.
  const MassMatrix<CoordinateSystemA> massMatrix{1.0, 1.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 3.0};
  EXPECT_TRUE(composition.transform(massMatrix).isApprox(swapCB.transform(scaling.transform(massMatrix)), 1e-12));
  EXPECT_TRUE(composition.transform(massMatrix)
                  .isApprox(MassMatrix<CoordinateSystemC>{0.125, 0.125, 0.0, 0.125, 0.25, 0.0, 0.0, 0.0, 0.046875}, 1e-12));
}

// The inverse transforms from To to From. It is the matrix inverse.
TEST(TestPassiveLinearTransformation, Inverse) {  // NOLINT
  using ScalingAB = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA, 3, Dimensionless>;
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const ScalingAB inverse = scaling.inverse();
  EXPECT_TRUE(inverse.isApprox(ScalingAB{0.5, 0.0, 0.0, 0.0, 0.25, 0.0, 0.0, 0.0, 0.125}, 1e-12));
  EXPECT_TRUE(inverse.transform(scaling.transform(vector)).isApprox(vector, 1e-12));
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemB> pointInB{1.0, 2.0, 3.0};
  EXPECT_TRUE(inverse.transform(scaling.transform(point)).isApprox(point, 1e-12));
  EXPECT_TRUE(scaling.transform(inverse.transform(pointInB)).isApprox(pointInB, 1e-12));
  using ScalingAA = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, 3, Dimensionless>;
  using ScalingBB = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemB, CoordinateSystemB, 3, Dimensionless>;
  EXPECT_TRUE((inverse * scaling).isApprox(ScalingAA{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, 1e-12));
  EXPECT_TRUE((scaling * inverse).isApprox(ScalingBB{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0}, 1e-12));
}

// The rows of the matrix are the dual basis covectors of To, and the columns of its inverse the basis vectors of To, both
// expressed in From: for the scaling, the basis vectors of B are (0.5, 0, 0), (0, 0.25, 0) and (0, 0, 0.125) in A. Each basis
// vector of B has the components of a unit vector in B.
TEST(TestPassiveLinearTransformation, BasisVectorsOfToAreColumnsOfTheInverse) {  // NOLINT
  using Direction = DirectionVector3<ValueMatrix, double, CoordinateSystemA>;
  using DirectionInB = DirectionVector3<ValueMatrix, double, CoordinateSystemB>;
  EXPECT_EQ(scaling.transform(Direction{0.5, 0.0, 0.0}), (DirectionInB{1.0, 0.0, 0.0}));
  EXPECT_EQ(scaling.transform(Direction{0.0, 0.25, 0.0}), (DirectionInB{0.0, 1.0, 0.0}));
  EXPECT_EQ(scaling.transform(Direction{0.0, 0.0, 0.125}), (DirectionInB{0.0, 0.0, 1.0}));
}
