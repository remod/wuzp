// wuzp-expect: error
// wuzp-static-assert: Only a square Jacobian, between coordinate systems of the same dimension, has an inverse\.

// A Jacobian between coordinate systems of different dimensions, e.g. of the task space of a robot with respect to its joint
// space, has no inverse. The pullback, e.g. of a task space force or mass matrix, needs none.

#include "wuzp/trafo/Jacobian.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

using namespace wuzp;  // NOLINT

using JacobianBA = Jacobian<ValueMatrix, double, CoordinateSystemA, CoordinateSystemB, 3, 2, Dimensionless>;

int main() {
  const JacobianBA jacobian{1.0, 0.0, 0.0, 0.0, 1.0, 0.0};
  [[maybe_unused]] const auto result = jacobian.inverse();
  return 0;
}
