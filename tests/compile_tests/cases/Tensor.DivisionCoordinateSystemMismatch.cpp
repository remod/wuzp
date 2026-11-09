// wuzp-expect: error
// wuzp-static-assert: Coordinate systems do not match\.

// The quotient by a tensor of rank 0 requires tensors in the same coordinate system (#61).

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>>::zero() / TestTensor<Indices<>, wuzp::Relative, CoordinateSystemB>::zero();
  return 0;
}
