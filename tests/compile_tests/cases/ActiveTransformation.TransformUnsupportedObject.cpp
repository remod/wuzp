// wuzp-expect: error
// wuzp-static-assert: 'TransformationTraits' trait not specialized\.

// Transforming an object that the transformation does not support, e.g. a plain number, fails with a clear message.

#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Rotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;

int main() {
  const Rotation rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = rotation.transform(1.0);
  return 0;
}
