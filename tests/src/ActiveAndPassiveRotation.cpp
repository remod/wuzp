#include <gtest/gtest.h>

#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"
#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestActiveAndPassiveRotation, CancelEachOtherWhenTransformingPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using ActiveTransformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using PassiveTransformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const ActiveTransformation activeTransformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};    // Active rotation of pi/2 around x.
  const PassiveTransformation passiveTransformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(passiveTransformation.transform(activeTransformation.transform(object)), transformedObject);
}

TEST(TestActiveAndPassiveRotation, CancelEachOtherWhenTransformingPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using ActiveTransformation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
  using PassiveTransformation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const ActiveTransformation activeTransformation{1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0};    // Active rotation of pi/2 around x.
  const PassiveTransformation passiveTransformation{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};  // Passive rotation of pi/2 around x.
  EXPECT_EQ(passiveTransformation.transform(activeTransformation.transform(object)), transformedObject);
}