// wuzp-expect: error
// wuzp-static-assert: Matrix sizes do not match\.

// A translation leaves only vectors of its own dimension unchanged, like it only moves points of its own dimension: the
// translation of a vector in the plane by a translation in space used to return the vector unchanged.

#include "wuzp/trafo/ActiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA> translation{0.0, 0.0, 1.0};
  const VelocityVector2<ValueMatrix, double, CoordinateSystemA> velocity{1.0, 2.0};
  [[maybe_unused]] const auto result = translation.transform(velocity);
  return 0;
}
