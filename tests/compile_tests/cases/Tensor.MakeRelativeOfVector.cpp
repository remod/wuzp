// wuzp-expect: error
// wuzp-static-assert: The object is already relative\.

// makeRelative() takes a point, and returns the vector from the origin to it. Calling it with a vector is a mistake.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = wuzp::makeRelative(TestTensor<Indices<Up>>::zero());
  return 0;
}
