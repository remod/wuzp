// wuzp-expect: error
// wuzp-static-assert: Adjacent frames do not match\.

// X_AB * X_BC = X_AC: the coordinate system From of the left-hand side must be To of the right-hand side. X_AB * X_CB, i.e. the
// passive transformation from B to C followed by the one from B to A, is a mistake.

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemA> rotationAB{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                      0.0, 0.0, 0.0, 1.0};
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC> rotationCB{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                      0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = rotationAB * rotationCB;
  return 0;
}
