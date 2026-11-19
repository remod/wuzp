// wuzp-expect: error
// wuzp-static-assert: Matrix sizes do not match\.

// A translation leaves only covectors of its own dimension unchanged, like it only moves points of its own dimension: the
// translation of a covector in the plane by a translation in space used to return the covector unchanged.

#include "wuzp/trafo/PassiveTranslation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> translation{0.0, 0.0, -1.0};
  const ForceCovector2<ValueMatrix, double, CoordinateSystemA> force{1.0, 2.0};
  [[maybe_unused]] const auto result = translation.transform(force);
  return 0;
}
