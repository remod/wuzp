// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// A spatial acceleration and a spatial velocity cannot be added, even with the same tags (#76).

#include "wuzp/trafo/SpatialAcceleration.hpp"

#include "spatial_vector_types.hpp"

int main() {
  const SpatialAccelerationD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> acceleration{
      AngularAccelerationInA{0.0, 0.0, 1.0}, AccelerationInA{1.0, 0.0, 0.0}};
  const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocity{AngularVelocityInA{0.0, 0.0, 1.0},
                                                                                                   VelocityInA{1.0, 0.0, 0.0}};
  [[maybe_unused]] const auto sum = acceleration + velocity;
  return 0;
}
