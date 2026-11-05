// wuzp-expect: error
// wuzp-static-assert: Cross product is only defined for vectors\.

// The cross product is only defined for vectors, not for covectors.

#include "compile_test_types.hpp"

int main() {
  using Lhs = wuzp::PositionVector3<ValueMatrix, double, CoordinateSystemA>;
  using Rhs = wuzp::HomogeneousCovector<3, ValueMatrix, double, Length, CoordinateSystemA>;
  [[maybe_unused]] const auto result = Lhs::zero().crossProduct(Rhs::zero());
  return 0;
}
