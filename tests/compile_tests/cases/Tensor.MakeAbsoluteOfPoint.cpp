// wuzp-expect: error
// wuzp-static-assert: The object is already absolute\.

// makeAbsolute() takes a vector, and returns the point at it from the origin. Calling it with a point is a mistake.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::makeAbsolute(TestTensor<Indices<Up>, wuzp::Absolute>::zero());
  return 0;
}
