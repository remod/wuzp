// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A proper rigid transformation consists of a rotation, which needs the Euclidean metric, so it is expressed in a Euclidean
// coordinate system.

#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"

#include "compile_test_types.hpp"

using Rotation = wuzp::ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
using Translation = wuzp::ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::ActiveProperRigidTransformationTemplate<SpacetimeA, Rotation, Translation>);
  return 0;
}
