// wuzp-expect: error
// wuzp-static-assert: Tensors of rank 3 or more are not supported at the moment\.

// Only tensors up to rank 2 are supported.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(TestTensor<2, 1>);
  return 0;
}
