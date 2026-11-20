// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// The angular velocity comes first, like in Drake and Modern Robotics: swapped parts do not compile (#76).

#include "wuzp/trafo/SpatialVelocity.hpp"

#include "spatial_vector_types.hpp"

int main() {
  [[maybe_unused]] const SpatialVelocityD<CoordinateSystemA, CoordinateSystemB, PointP, CoordinateSystemA> velocity{
      VelocityInA{1.0, 0.0, 0.0}, AngularVelocityInA{0.0, 0.0, 1.0}};
  return 0;
}
