// wuzp-expect: error
// wuzp-static-assert: The common type of the scalar and the value type is not the value type, e\.g\. a double scalar with int values\.

// A scalar is converted into the value type of the tensor, e.g. 2 * v for a vector of doubles, but not if this narrows it.

#include "compile_test_types.hpp"

int main() {
  using Vector = wuzp::PositionVector3<ValueMatrix, int, CoordinateSystemA>;
  [[maybe_unused]] const auto result = Vector::zero() * 2.5;
  return 0;
}
