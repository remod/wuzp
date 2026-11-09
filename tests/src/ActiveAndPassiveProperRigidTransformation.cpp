#include <gtest/gtest.h>

#include "wuzp/core/Absolute.hpp"
#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

// The active transformation G = (R, t) moves the coordinate system A onto B: the axes of B are those of A rotated by R, and the
// origin of B is at t in A. The passive transformation from A to B with the same parameters, the rotation R and the translation
// t, cancels it: it is its inverse, X_BA = (R^T, -R^T t), whose translation -R^T t is expressed in B (see DESIGN.md).

namespace {

using ActiveRigidTransformation = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
using PassiveRigidTransformation = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;

// An active rotation of pi/2 around x, then a translation of 1 along z.
const ActiveRigidTransformation activeTransformation{ActiveRigidTransformation::Rotation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0},
                                                     ActiveRigidTransformation::Translation{0.0, 0.0, 1.0}};
// A passive rotation of pi/2 around x, R^T, and the translation -R^T t = -(0, 1, 0), expressed in B.
const PassiveRigidTransformation passiveTransformation{PassiveRigidTransformation::Rotation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0},
                                                       PassiveRigidTransformation::Translation{0.0, -1.0, 0.0}};

}  // namespace

TEST(TestActiveAndPassiveProperRigidTransformation, CancelEachOtherWhenTransformingPositionPoint) {  // NOLINT
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> object{1.0, 2.0, 3.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemB> transformedObject{1.0, 2.0, 3.0};
  EXPECT_EQ(passiveTransformation.transform(activeTransformation.transform(object)), transformedObject);
}

TEST(TestActiveAndPassiveProperRigidTransformation, CancelEachOtherWhenTransformingPositionVector) {  // NOLINT
  const PositionVector3<ValueMatrix, double, CoordinateSystemA> object{1.0, 2.0, 3.0};
  const PositionVector3<ValueMatrix, double, CoordinateSystemB> transformedObject{1.0, 2.0, 3.0};
  EXPECT_EQ(passiveTransformation.transform(activeTransformation.transform(object)), transformedObject);
}

// The passive transformation has the rotation and the translation of the inverse of the active one.
TEST(TestActiveAndPassiveProperRigidTransformation, PassiveIsTheInverseOfActive) {  // NOLINT
  const ActiveRigidTransformation inverse = activeTransformation.inverse();
  EXPECT_EQ(passiveTransformation.getRotation().getQuantities(), inverse.getRotation().getQuantities());
  EXPECT_EQ(passiveTransformation.getTranslation().getQuantities(), inverse.getTranslation().getQuantities());
}

// The translation -t, which a passive translation with the same parameters as an active translation has, does not cancel a
// rotation and a translation, unless R t = t: (1, 2, 3) becomes (1, 3, 2).
TEST(TestActiveAndPassiveProperRigidTransformation, NegatedTranslationDoesNotCancel) {  // NOLINT
  const PassiveRigidTransformation negatedTranslation{passiveTransformation.getRotation(),
                                                      PassiveRigidTransformation::Translation{0.0, 0.0, -1.0}};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> object{1.0, 2.0, 3.0};
  EXPECT_EQ(negatedTranslation.transform(activeTransformation.transform(object)),
            (PositionPoint3<ValueMatrix, double, CoordinateSystemB>{1.0, 3.0, 2.0}));
}
