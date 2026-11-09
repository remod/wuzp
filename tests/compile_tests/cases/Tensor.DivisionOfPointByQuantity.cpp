// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a division\.

// The quotient of a point and a quantity depends on the origin.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>, wuzp::Absolute>::zero() / QuantityD<Length>{2.0};
  return 0;
}
