// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The rotation of a proper rigid transformation is a rotation, whose inverse is its transpose, not e.g. a linear transformation
// with dimensionless units, which may be a scaling.

#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"
#include "wuzp/trafo/ActiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using LinearTransformation = ActiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, 3, 3, Dimensionless>;
using Translation = ActiveSpatialTranslation3<ValueMatrix, double, CoordinateSystemA>;

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(ActiveProperRigidTransformationTemplate<CoordinateSystemA, LinearTransformation, Translation>);
  return 0;
}
