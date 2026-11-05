// wuzp-expect: error
// wuzp-static-assert: The translation must be expressed in the coordinate system From or To\.

// A translation from A to B expressed in a third coordinate system C would need its components transformed first.

#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA, CoordinateSystemB> translation{0.0, 0.0, 1.0};
  const PositionPoint3<ValueMatrix, double, CoordinateSystemA> point{1.0, 2.0, 3.0};
  [[maybe_unused]] const auto result = translation.transform(point);
  return 0;
}
