// wuzp-expect: error
// wuzp-static-assert: A tensor of rank 0 is stored as a 1x1 matrix\.

// A tensor of rank 0 has a single component.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::Tensor<Indices<>, wuzp::Quantities<ValueMatrix<double, 2, 1>, wuzp::TypeMatrix<2, 1, Length>>, CoordinateSystemA,
                          wuzp::Relative>);
  return 0;
}
