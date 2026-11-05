// wuzp-expect: error
// wuzp-requires: values=eigen
// wuzp-static-assert: Tensor product is not implemented for eigen\.

// The backend does not implement the tensor product. Through the public API, generic checks report the problem first, so the
// backend trait is used directly.

#include "ValueMatrix.hpp"

int main() {
  using Implementation = wuzp::AccessImplementation<ValueMatrix<double, 3, 1>::ImplementationT>;
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::TensorProduct<Implementation, Implementation>);
  return 0;
}
