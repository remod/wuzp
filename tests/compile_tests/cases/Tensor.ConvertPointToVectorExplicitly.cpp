// wuzp-expect: compiles

// Converting between points and vectors is possible, but only explicitly.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const TestTensor<Indices<Up>> vector = wuzp::makeRelative(TestTensor<Indices<Up>, wuzp::Absolute>::zero());
  [[maybe_unused]] const TestTensor<Indices<Up>, wuzp::Absolute> point = wuzp::makeAbsolute(TestTensor<Indices<Up>>::zero());
  return 0;
}
