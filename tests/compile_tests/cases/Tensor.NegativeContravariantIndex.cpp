// wuzp-expect: error
// wuzp-static-assert: Contravariant index must not be negative\.

// Tensor indices are counts and cannot be negative.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(TestTensor<-1, 0>);
  return 0;
}
