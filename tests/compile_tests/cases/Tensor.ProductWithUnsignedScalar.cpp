// wuzp-expect: error
// wuzp-static-assert: The common type of the scalar and the value type is not the value type, e\.g\. a double scalar with int values\.

// An unsigned scalar is not converted into int, the value type of the tensor: their common type is unsigned, and an unsigned
// value above INT_MAX would change when converted into int.

#include "compile_test_types.hpp"

int main() {
  using Vector = wuzp::PositionVector3<ValueMatrix, int, CoordinateSystemA>;
  [[maybe_unused]] const auto result = Vector::zero() * 2U;
  return 0;
}
