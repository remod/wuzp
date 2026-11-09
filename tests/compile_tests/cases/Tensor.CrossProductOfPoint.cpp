// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// The cross product of a point depends on the origin, so it is not defined, only of vectors.

#include "compile_test_types.hpp"

int main() {
  using Lhs = wuzp::PositionPoint3<ValueMatrix, double, CoordinateSystemA>;
  using Rhs = wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  [[maybe_unused]] const auto result = Lhs::zero().crossProduct(Rhs::zero());
  return 0;
}
