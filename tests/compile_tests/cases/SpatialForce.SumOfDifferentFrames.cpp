// wuzp-expect: error
// wuzp-static-assert: Frames do not match\.

// The spatial forces on two different frames, e.g. two bodies, cannot be added (#76).

#include "wuzp/trafo/SpatialForce.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemA> forceOnB{TorqueInA{0.0, 0.0, 1.0}, ForceInA{1.0, 0.0, 0.0}};
  const SpatialForceD<CoordinateSystemC, PointP, CoordinateSystemA> forceOnC{TorqueInA{0.0, 1.0, 0.0}, ForceInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto sum = forceOnB + forceOnC;
  return 0;
}
