// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot appear in a multiplication\.

// The product of a point and a quantity depends on the origin.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = QuantityD<Length>{2.0} * TestTensor<Indices<Up>, wuzp::Absolute>::zero();
  return 0;
}
