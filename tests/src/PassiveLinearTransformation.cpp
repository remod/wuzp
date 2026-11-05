#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

// A general linear scaling, which is not orthogonal: A^-1 != A^T.
using Scaling =
    PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB, 3, 3, Dimensionless>;

template <typename CoordinateSystemT>
using ForceCovector = HomogeneousCovector<3, ValueMatrix, double, Force, CoordinateSystemT>;
template <typename CoordinateSystemT>
using Covariance = Tensor<2, 0, Quantities<ValueMatrix<double, 3, 3>, TypeMatrix<3, 3, Area>>, CoordinateSystemT, Relative>;

const Scaling scaling{2.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 8.0};  // Scaling by 2, 4 and 8.

#if VALUES_IMPLEMENTATION != VALUES_IMPLEMENTATION_BLITZ
TEST(TestPassiveLinearTransformation, TransformCovector) {  // NOLINT
  // A covector transforms with the inverse: w' = w A^-1.
  const ForceCovector<CoordinateSystemA> object{8.0, 8.0, 8.0};
  const ForceCovector<CoordinateSystemB> transformedObject{4.0, 2.0, 1.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject));
}

TEST(TestPassiveLinearTransformation, TransformCopoint) {  // NOLINT
  // An absolute covector transforms like a relative one, since a linear transformation has no translation.
  const HomogeneousCopoint<3, ValueMatrix, double, Force, CoordinateSystemA> object{8.0, 8.0, 8.0};
  const HomogeneousCopoint<3, ValueMatrix, double, Force, CoordinateSystemB> transformedObject{4.0, 2.0, 1.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject));
}

TEST(TestPassiveLinearTransformation, TransformCovectorAndVectorPreservesTheirPairing) {  // NOLINT
  // The work done by a force along a displacement does not depend on the coordinates: w' v' = w A^-1 A v = w v.
  const ForceCovector<CoordinateSystemA> force{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> displacement{4.0, 5.0, 6.0};
  const QuantityD<Energy> work{innerProduct(force, displacement).getQuantity()};
  const QuantityD<Energy> transformedWork{innerProduct(scaling.transform(force), scaling.transform(displacement)).getQuantity()};
  EXPECT_TRUE(transformedWork.isApprox(work));
  EXPECT_TRUE(work.isApprox(QuantityD<Energy>{32.0}));
}
#endif

TEST(TestPassiveLinearTransformation, TransformTensorWithTwoContravariantIndices) {  // NOLINT
  // A covariance transforms like T' = A T A^T, i.e. T'_ij = a_i a_j T_ij for a scaling A = diag(a).
  const Covariance<CoordinateSystemA> object{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  const Covariance<CoordinateSystemB> transformedObject{4.0, 8.0, 16.0, 8.0, 16.0, 32.0, 16.0, 32.0, 64.0};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject));
}

TEST(TestPassiveLinearTransformation, TransformOuterProductOfVectors) {  // NOLINT
  // The outer product of a vector with itself transforms like the outer product of the transformed vectors.
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> vector{1.0, 2.0, 3.0};
  const Covariance<CoordinateSystemA> object{outerProduct(vector, vector.transpose())};
  const auto transformedVector = scaling.transform(vector);
  const Covariance<CoordinateSystemB> transformedObject{outerProduct(transformedVector, transformedVector.transpose())};
  EXPECT_TRUE(scaling.transform(object).isApprox(transformedObject));
}
