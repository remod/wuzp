// wuzp-expect: error
// wuzp-static-assert: Indices do not match\.

// A tensor cannot be converted into a tensor with other indices.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<Indices<Up>> tensor{TestTensor<Indices<>>::zero()};
  return 0;
}
