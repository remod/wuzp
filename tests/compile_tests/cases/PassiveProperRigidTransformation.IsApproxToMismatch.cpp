// wuzp-expect: error
// wuzp-static-assert: 'To' coordinate systems do not match\.

// Passive proper rigid transformations can only be compared approximately if they transform between the same coordinate systems
// (#68).

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using TransformationBA = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemB, double>;
using TransformationCA = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, CoordinateSystemC, double>;

int main() {
  const TransformationBA lhs{TransformationBA::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                             TransformationBA::Translation{0.0, 0.0, 1.0}};
  const TransformationCA rhs{TransformationCA::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                             TransformationCA::Translation{0.0, 0.0, 1.0}};
  return lhs.isApprox(rhs, 1e-12) ? 0 : 1;
}
