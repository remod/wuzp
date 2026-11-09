// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Only transformations whose matrices are expressed in the same coordinate system can be compared.

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemB> lhs{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                                  0.0, 0.0, 0.0, 1.0};
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA, CoordinateSystemB> rhs{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                                  0.0, 0.0, 0.0, 1.0};
  return lhs == rhs ? 0 : 1;
}
