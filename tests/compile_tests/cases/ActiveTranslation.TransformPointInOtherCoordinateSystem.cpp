// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// A translation expressed in B cannot move a point expressed in A.

#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB> translation{0.0, 0.0, 1.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto result = translation.transform(point);
  return 0;
}
