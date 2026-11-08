// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A passive proper rigid transformation is expressed in a coordinate system, not in the identifier of one.

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Rotation = PassiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA, CoordinateSystemA, CoordinateSystemC>;
using Translation = PassiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemB, CoordinateSystemC, CoordinateSystemB>;

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(PassiveProperRigidTransformationTemplate<CoordinateSystemAId, CoordinateSystemA, CoordinateSystemB, Rotation, Translation>);
  return 0;
}
