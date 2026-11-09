// wuzp-expect: error
// wuzp-static-assert: A passive transformation transforms tensors, not transformations\. Compose transformations with operator\*\.

// A passive transformation transforms tensors expressed in From, not other passive transformations, which are not expressed in
// a single coordinate system: re-expressing them produced transformations that could not be applied (#73). Compose with
// operator* and inverse() instead.

#include "wuzp/trafo/PassiveSpatialRotation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

int main() {
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB> rotationBA{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                      0.0, 0.0, 0.0, 1.0};
  const PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemA> rotationAC{1.0, 0.0, 0.0, 0.0, 1.0,
                                                                                                      0.0, 0.0, 0.0, 1.0};
  [[maybe_unused]] const auto result = rotationBA.transform(rotationAC);
  return 0;
}
