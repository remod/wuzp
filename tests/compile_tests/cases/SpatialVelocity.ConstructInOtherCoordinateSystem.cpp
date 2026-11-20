// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Both parts of a spatial velocity are expressed in its coordinate system (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  [[maybe_unused]] const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocity{
      wuzp::AngularVelocityVector3<ValueMatrix, double, CoordinateSystemB>{0.0, 0.0, 1.0}, VelocityInA{1.0, 0.0, 0.0}};
  return 0;
}
