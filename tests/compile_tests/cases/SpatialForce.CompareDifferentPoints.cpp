// wuzp-expect: error
// wuzp-static-assert: Reference points do not match\.

// Spatial forces are only compared if they are about the same point: the torques about different points differ even for the same
// force (#76).

#include "wuzp/trafo/SpatialForce.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemA> forceAboutP{TorqueInA{0.0, 0.0, 1.0}, ForceInA{1.0, 0.0, 0.0}};
  const SpatialForceD<CoordinateSystemB, PointQ, CoordinateSystemA> forceAboutQ{TorqueInA{0.0, 0.0, 1.0}, ForceInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const bool equal = forceAboutP == forceAboutQ;
  return 0;
}
