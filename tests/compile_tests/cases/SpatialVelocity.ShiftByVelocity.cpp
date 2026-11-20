// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// shift() takes the position vector from the point to the other point, not a velocity (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocity{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                   VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto shifted = velocity.shift<PointQ>(VelocityInA{1.0, 0.0, 0.0});
  return 0;
}
