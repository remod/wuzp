// wuzp-expect: compiles

// Converting between points and vectors is possible, but only explicitly.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<1, 0> vector = wuzp::makeRelative(TestTensor<1, 0, wuzp::Absolute>::zero());
  [[maybe_unused]] const TestTensor<1, 0, wuzp::Absolute> point = wuzp::makeAbsolute(TestTensor<1, 0>::zero());
  return 0;
}
