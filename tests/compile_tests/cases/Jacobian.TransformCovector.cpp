// wuzp-expect: error
// wuzp-static-assert: A Jacobian pushes forward vectors and tensors with two upper indices\. Use pullback\(\) for covectors and tensors with two lower indices\.

// A covector, e.g. a force, is mapped the other way, from To to From, by the pullback w J, which needs no inverse: e.g. a force at
// the end effector of a robot into joint torques.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 2, 2, Dimensionless>;

int main() {
  const JacobianBA jacobian{0.0, -1.0, 1.0, 0.0};
  const HomogeneousCovector<2, ValueMatrix, double, Force, CoordinateSystemA> force{1.0, 2.0};
  [[maybe_unused]] const auto result = jacobian.transform(force);
  return 0;
}
