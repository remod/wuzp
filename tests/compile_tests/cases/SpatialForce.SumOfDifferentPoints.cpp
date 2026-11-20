// wuzp-expect: error
// wuzp-static-assert: Reference points do not match\.

// Torques about different points cannot be added: shift() one of the spatial forces to the point of the other (#76).

#include "wuzp/trafo/SpatialForce.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialForceD<CoordinateSystemB, PointP, CoordinateSystemA> forceAboutP{TorqueInA{0.0, 0.0, 1.0}, ForceInA{1.0, 0.0, 0.0}};
  const SpatialForceD<CoordinateSystemB, PointQ, CoordinateSystemA> forceAboutQ{TorqueInA{0.0, 1.0, 0.0}, ForceInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto sum = forceAboutP + forceAboutQ;
  return 0;
}
