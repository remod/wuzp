// wuzp-expect: error
// wuzp-static-assert: 'TransformationTraits' trait not specialized\.

// Only passive transformations re-express spatial velocities: an active rotation would rotate the motion, which the frames and the
// point of the spatial velocity would no longer describe (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"
#include "wuzp/trafo/ActiveSpatialRotation.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA> rotation{};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocity{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                   VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto transformed = rotation.transform(velocity);
  return 0;
}
