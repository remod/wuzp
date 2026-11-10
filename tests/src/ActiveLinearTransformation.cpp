#include <gtest/gtest.h>

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/TypeMatrix.hpp"
#include "wuzp/trafo/ActiveLinearTransformation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "to_string.hpp"

using namespace wuzp;  // NOLINT

// A general linear scaling, which is not orthogonal: A^-1 != A^T.
using Scaling = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Dimensionless>;

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
const Scaling swapXAndY{0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

TEST(TestActiveLinearTransformation, StreamOperator) {  // NOLINT
  EXPECT_EQ(toString(scaling), toString(scaling.getTensor()));
}

// Covectors and (1,1) tensors transform with the inverse.
TEST(TestActiveLinearTransformation, TransformCovector) {  // NOLINT
  // A covector transforms with the inverse: w' = w A^-1.
  const ForceCovector<CoordinateSystemA> object{8.0, 8.0, 8.0};
  const ForceCovector<CoordinateSystemA> transformedObject{4.0, 2.0, 1.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject, 1e-12));
}

TEST(TestActiveLinearTransformation, TransformCovectorAndVectorPreservesTheirPairing) {  // NOLINT
  // The work done by a force along a displacement does not depend on the coordinates: w' v' = w A^-1 A v = w v.
  const ForceCovector<CoordinateSystemA> force{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> displacement{4.0, 5.0, 6.0};
  const QuantityD<Energy> work{innerProduct(force, displacement).getQuantity()};
  const QuantityD<Energy> transformedWork{innerProduct(scaling.transform(force), scaling.transform(displacement)).getQuantity()};
  EXPECT_TRUE(transformedWork.isApprox(work, 1e-12));
  EXPECT_TRUE(work.isApprox(QuantityD<Energy>{32.0}, 1e-12));
}

TEST(TestActiveLinearTransformation, TransformTensorWithOneContravariantAndOneCovariantIndex) {  // NOLINT
  // A linear map, e.g. a velocity gradient, transforms like T' = A T A^-1, i.e. T'_ij = a_i / a_j T_ij for a scaling A = diag(a).
  const VelocityGradient<CoordinateSystemA> object{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const VelocityGradient<CoordinateSystemA> transformedObject{1.0, 0.5, 0.25, 2.0, 1.0, 0.5, 4.0, 2.0, 1.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject, 1e-12));
}

TEST(TestActiveLinearTransformation, TransformTensorWithTwoContravariantIndices) {  // NOLINT
  // A covariance transforms like T' = A T A^T, i.e. T'_ij = a_i a_j T_ij for a scaling A = diag(a).
  const Covariance<CoordinateSystemA> object{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const Covariance<CoordinateSystemA> transformedObject{4.0, 8.0, 16.0, 8.0, 16.0, 32.0, 16.0, 32.0, 64.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject, 1e-12));
}

TEST(TestActiveLinearTransformation, TransformOuterProductOfVectors) {  // NOLINT
  // The outer product of a vector with itself transforms like the outer product of the transformed vectors.
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const Covariance<CoordinateSystemA> object{outerProduct(vector, vector)};
  const auto transformedVector = scaling.transform(vector);
  const Covariance<CoordinateSystemA> transformedObject{outerProduct(transformedVector, transformedVector)};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject, 1e-12));
}

namespace {

template <typename TransformationT_>
concept HasGetTensor = requires(const TransformationT_& transformation) { transformation.getTensor(); };

}  // namespace

// The matrix of an active linear transformation is a linear map, a tensor with an upper and a lower index in its coordinate
// system, if it is square: getTensor() only exists for square matrices, getQuantities() for all.
TEST(TestActiveLinearTransformation, MatrixIsALinearMapOnlyIfSquare) {  // NOLINT
  using NonSquare = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 2, Dimensionless>;
  static_assert(!HasGetTensor<NonSquare>);
  static_assert(HasGetTensor<Scaling>);
  const NonSquare nonSquare{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  EXPECT_EQ(nonSquare.getQuantities(),
            (Quantities<ValueMatrix<double, 2, 3>, TypeMatrix<2, 3, Dimensionless>>{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}));
  EXPECT_EQ(scaling.getTensor(),
            (Tensor<Indices<Up, Down>, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Dimensionless>>, CoordinateSystemA, Relative>{
                2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0}));
}

// A transformation can be copied from a non-const object: the forwarding constructor does not hijack the copy (#66).
TEST(TestActiveLinearTransformation, CopyFromNonConstObject) {  // NOLINT
  Scaling transformation{scaling};
  const Scaling copy(transformation);
  EXPECT_EQ(copy, scaling);
}

// isApprox is const and takes a precision, like that of tensors (#68).
TEST(TestActiveLinearTransformation, IsApprox) {  // NOLINT
  EXPECT_TRUE(scaling.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0 + 1e-14}));
  EXPECT_FALSE(scaling.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 9.0}));
  EXPECT_TRUE(scaling.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.001}, 1e-3));
  EXPECT_FALSE(scaling.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.001}, 1e-6));
}

// The composition is the product of the matrices, and applies the right-hand side first. The units of the product follow from
// those of the factors, e.g. a velocity gradient times a time is dimensionless.
TEST(TestActiveLinearTransformation, Compose) {  // NOLINT
  using FrequencyMap = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Frequency>;
  using TimeMap = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Time>;
  const FrequencyMap frequencyMap{1.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 3.0};
  const TimeMap timeMap{0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const Scaling composition = frequencyMap * timeMap;
  EXPECT_EQ(composition, (Scaling{0.0, 1.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 3.0}));
  EXPECT_EQ(composition.transform(vector), frequencyMap.transform(timeMap.transform(vector)));
}

// Non-square matrices compose if their dimensions match: a map from 3 to 2 dimensions after one from 2 to 3 dimensions.
TEST(TestActiveLinearTransformation, ComposeNonSquare) {  // NOLINT
  using Projection = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 2, Dimensionless>;
  using Embedding = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 2, 3, Dimensionless>;
  using Square2 = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 2, 2, Dimensionless>;
  using Square3 = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Dimensionless>;
  const Projection projection{1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
  const Embedding embedding{1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
  EXPECT_EQ(projection * embedding, (Square2{1.0, 0.0, 0.0, 1.0}));
  EXPECT_EQ(embedding * projection, (Square3{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0}));
}

// The composition transforms every tensor like applying the right-hand side first, then the left-hand side: here the scaling
// diag(2, 4, 8) after swapping x and y, i.e. the matrix A = ((0, 2, 0), (4, 0, 0), (0, 0, 8)). v' = A v, and T' = A T A^T.
TEST(TestActiveLinearTransformation, ComposeTransformsPointsVectorsAndTensors) {  // NOLINT
  const Scaling composition = scaling * swapXAndY;
  EXPECT_EQ(composition, (Scaling{0.0, 2.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 8.0}));
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  EXPECT_EQ(composition.transform(point), scaling.transform(swapXAndY.transform(point)));
  EXPECT_EQ(composition.transform(point), (PositionPoint3<ValueMatrix, double, CoordinateSystemA>{4.0, 4.0, 24.0}));
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  EXPECT_EQ(composition.transform(vector), scaling.transform(swapXAndY.transform(vector)));
  EXPECT_EQ(composition.transform(vector), (PositionVector3<ValueMatrix, double, CoordinateSystemA>{4.0, 4.0, 24.0}));
  const Covariance<CoordinateSystemA> covariance{1.0, 1.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 3.0};
  EXPECT_EQ(composition.transform(covariance), scaling.transform(swapXAndY.transform(covariance)));
  EXPECT_EQ(composition.transform(covariance), (Covariance<CoordinateSystemA>{8.0, 8.0, 0.0, 8.0, 16.0, 0.0, 0.0, 0.0, 192.0}));
}

// Covectors and tensors with lower indices transform with the inverse. The inverse of the composition is ((0, 1/4, 0), (1/2, 0, 0), (0, 0,
// 1/8)).
TEST(TestActiveLinearTransformation, ComposeTransformsCovectorsAndTensorsWithLowerIndices) {  // NOLINT
  const Scaling composition = scaling * swapXAndY;
  // w' = w A^-1.
  const ForceCovector<CoordinateSystemA> covector{8.0, 16.0, 8.0};
  EXPECT_TRUE(composition.transform(covector).isApprox(scaling.transform(swapXAndY.transform(covector)), 1e-12));
  EXPECT_TRUE(composition.transform(covector).isApprox(ForceCovector<CoordinateSystemA>{8.0, 2.0, 1.0}, 1e-12));
  // T' = A T A^-1.
  const VelocityGradient<CoordinateSystemA> velocityGradient{1.0, 1.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 3.0};
  EXPECT_TRUE(composition.transform(velocityGradient).isApprox(scaling.transform(swapXAndY.transform(velocityGradient)), 1e-12));
  EXPECT_TRUE(composition.transform(velocityGradient)
                  .isApprox(VelocityGradient<CoordinateSystemA>{2.0, 0.5, 0.0, 2.0, 1.0, 0.0, 0.0, 0.0, 3.0}, 1e-12));
  // M' = A^-T M A^-1.
  const MassMatrix<CoordinateSystemA> massMatrix{1.0, 1.0, 0.0, 1.0, 2.0, 0.0, 0.0, 0.0, 3.0};
  EXPECT_TRUE(composition.transform(massMatrix).isApprox(scaling.transform(swapXAndY.transform(massMatrix)), 1e-12));
  EXPECT_TRUE(composition.transform(massMatrix)
                  .isApprox(MassMatrix<CoordinateSystemA>{0.5, 0.125, 0.0, 0.125, 0.0625, 0.0, 0.0, 0.0, 0.046875}, 1e-12));
}

// The inverse of a linear transformation is its matrix inverse.
TEST(TestActiveLinearTransformation, Inverse) {  // NOLINT
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const Scaling inverse = scaling.inverse();
  EXPECT_TRUE(inverse.isApprox(Scaling{0.5, 0.0, 0.0, 0.0, 0.25, 0.0, 0.0, 0.0, 0.125}, 1e-12));
  EXPECT_TRUE(inverse.transform(scaling.transform(vector)).isApprox(vector, 1e-12));
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  EXPECT_TRUE(inverse.transform(scaling.transform(point)).isApprox(point, 1e-12));
  EXPECT_TRUE(scaling.transform(inverse.transform(point)).isApprox(point, 1e-12));
  const Scaling identity{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  EXPECT_TRUE((scaling * inverse).isApprox(identity, 1e-12));
  EXPECT_TRUE((inverse * scaling).isApprox(identity, 1e-12));
}

// The units of the inverse are the inverse units of the transposed matrix (see Inverse<TypeMatrixImpl<...>>).
TEST(TestActiveLinearTransformation, InverseOfUnits) {  // NOLINT
  using FrequencyMap = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Frequency>;
  using TimeMap = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Time>;
  const FrequencyMap frequencyMap{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0};
  const TimeMap inverse = frequencyMap.inverse();
  EXPECT_TRUE(inverse.isApprox(TimeMap{0.5, 0.0, 0.0, 0.0, 0.25, 0.0, 0.0, 0.0, 0.125}, 1e-12));
}
