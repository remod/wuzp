#include <gtest/gtest.h>

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/traits.hpp"
#include "wuzp/trafo/ActiveTranslation.hpp"
#include "wuzp/trafo/PassiveTranslation.hpp"

#include "coordinate_systems.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

TEST(TestActiveAndPassiveTranslation, CancelEachOtherWhenTransformingPositionVector) {  // NOLINT
  using Object = PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using ActiveTransformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using PassiveTransformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionVector3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const ActiveTransformation activeTransformation{0.0, 0.0, 1.0};     // Active translation of 1 along z.
  const PassiveTransformation passiveTransformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(passiveTransformation.transform(activeTransformation.transform(object)), transformedObject);
}

TEST(TestActiveAndPassiveTranslation, CancelEachOtherWhenTransformingPositionPoint) {  // NOLINT
  using Object = PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using ActiveTransformation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;
  using PassiveTransformation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB>;
  using TransformedObject = PositionPoint3<ValueMatrix, double, CoordinateSystemB>;
  const Object object{1.0, 2.0, 3.0};
  const TransformedObject transformedObject{1.0, 2.0, 3.0};
  const ActiveTransformation activeTransformation{0.0, 0.0, 1.0};     // Active translation of 1 along z.
  const PassiveTransformation passiveTransformation{0.0, 0.0, -1.0};  // Passive translation of 1 along z.
  EXPECT_EQ(passiveTransformation.transform(activeTransformation.transform(object)), transformedObject);
}
