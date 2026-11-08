// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A proper rigid transformation is expressed in a coordinate system, not in the identifier of one.

#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Rotation = ActiveSpatialRotation3<ValueMatrix, double, CoordinateSystemA>;
using Translation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(ActiveProperRigidTransformationTemplate<CoordinateSystemAId, Rotation, Translation>);
  return 0;
}
