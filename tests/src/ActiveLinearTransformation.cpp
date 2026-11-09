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

const Scaling scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0};  // Scaling by 2, 4 and 8.

TEST(TestActiveLinearTransformation, StreamOperator) {  // NOLINT
  EXPECT_EQ(toString(scaling), toString(scaling.getTensor()));
}

// Covectors and (1,1) tensors transform with the inverse, which Blitz++ does not provide.
#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
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
#endif

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

// A transformation can be copied from a non-const object: the forwarding constructor does not hijack the copy (#66).
TEST(TestActiveLinearTransformation, CopyFromNonConstObject) {  // NOLINT
  Scaling transformation{scaling};
  const Scaling copy(transformation);
  EXPECT_EQ(copy, scaling);
}

TEST(TestActiveLinearTransformation, IsApprox) {  // NOLINT
  Scaling transformation{scaling};                // Not const: isApprox() is not const yet (#68).
  EXPECT_TRUE(transformation.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0 + 1e-14}));
  EXPECT_FALSE(transformation.isApprox(Scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 9.0}));
}
