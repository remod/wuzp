// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A passive proper rigid transformation transforms into a coordinate system, not into the identifier of one.

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Rotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemC>;
using Translation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemC, CoordinateSystemB>;

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(PassiveProperRigidTransformationTemplate<CoordinateSystemA, CoordinateSystemBId, Rotation, Translation>);
  return 0;
}
