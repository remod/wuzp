// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The rotation of a proper rigid transformation is a rotation, whose inverse is its transpose, not e.g. a linear transformation
// with dimensionless units, which may be a scaling.

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"
#include "wuzp/trafo/PassiveLinearTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using Intermediate = wuzp::internal::Intermediate<CoordinateSystemA, CoordinateSystemB>;
using LinearTransformation = PassiveLinearTransformation<ValueMatrix, double, CoordinateSystemA, Intermediate, 3, 3, Dimensionless>;
using Translation = PassiveSpatialTranslation3<ValueMatrix, double, Intermediate, CoordinateSystemB>;

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(PassiveProperRigidTransformationTemplate<CoordinateSystemA, CoordinateSystemB, LinearTransformation, Translation>);
  return 0;
}
