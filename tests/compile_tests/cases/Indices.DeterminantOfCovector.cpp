// wuzp-expect: error
// wuzp-static-assert: A vector or a covector has no determinant\.

// Only tensors of rank 0 and 2 have a determinant, like the inverse. The trait of the indices rejects a vector or a covector, also
// outside of Tensor::determinant() (see Tensor.DeterminantOfVector).

#include "wuzp/core/Indices.hpp"

int main() {
  using Type = wuzp::Determinant<wuzp::Indices<wuzp::Down>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
