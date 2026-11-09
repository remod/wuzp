// wuzp-expect: error
// wuzp-static-assert: 'From' coordinate systems do not match\.

// Passive proper rigid transformations can only be compared approximately if they transform between the same coordinate systems
// (#68).

#include "wuzp/trafo/PassiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using TransformationAB = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, CoordinateSystemA, double>;
using TransformationAC = PassiveProperRigidTransformation3<ValueMatrix, CoordinateSystemC, CoordinateSystemA, double>;

int main() {
  const TransformationAB lhs{TransformationAB::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                             TransformationAB::Translation{0.0, 0.0, 1.0}};
  const TransformationAC rhs{TransformationAC::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                             TransformationAC::Translation{0.0, 0.0, 1.0}};
  return lhs.isApprox(rhs, 1e-12) ? 0 : 1;
}
