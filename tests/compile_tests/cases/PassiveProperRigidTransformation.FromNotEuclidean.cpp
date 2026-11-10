// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A proper rigid transformation consists of a rotation, which needs the Euclidean metric, so it transforms from a Euclidean
// coordinate system.

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "compile_test_types.hpp"

using Rotation = wuzp::PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC>;
using Translation = wuzp::PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemB>;

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::PassiveProperRigidTransformationTemplate<SpacetimeA, CoordinateSystemB, Rotation, Translation>);
  return 0;
}
