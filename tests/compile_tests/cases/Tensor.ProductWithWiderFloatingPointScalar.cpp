// wuzp-expect: error
// wuzp-static-assert: The common type of the scalar and the value type is not the value type, e\.g\. a double scalar with int values\.

// A double scalar is not converted into float, the value type of the tensor, which could lose precision: a vector of floats is
// multiplied by a float scalar, e.g. v * 0.5F.

#include "compile_test_types.hpp"

int main() {
  using Vector = wuzp::PositionVector3<ValueMatrix, float, CoordinateSystemA>;
  [[maybe_unused]] const auto result = Vector::zero() * 0.5;
  return 0;
}
