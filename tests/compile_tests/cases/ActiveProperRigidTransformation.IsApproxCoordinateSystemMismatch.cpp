// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// Proper rigid transformations can only be compared approximately if they are expressed in the same coordinate system (#68).

#include "wuzp/trafo/ActiveProperRigidTransformation.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using TransformationInA = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemA, double>;
using TransformationInB = ActiveProperRigidTransformation3<ValueMatrix, CoordinateSystemB, double>;

int main() {
  const TransformationInA lhs{TransformationInA::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                              TransformationInA::Translation{0.0, 0.0, 1.0}};
  const TransformationInB rhs{TransformationInB::Rotation{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0},
                              TransformationInB::Translation{0.0, 0.0, 1.0}};
  return lhs.isApprox(rhs, 1e-12) ? 0 : 1;
}
