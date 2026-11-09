// wuzp-expect: error
// wuzp-static-assert: A tensor of rank 2 is stored as a square matrix\.

// Both indices of a tensor of rank 2 range over the dimension of the coordinate system.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::Tensor<Indices<Up, Down>, wuzp::Quantities<ValueMatrix<double, 2, 3>, wuzp::TypeMatrix<2, 3, Length>>, CoordinateSystemA,
                          wuzp::Relative>);
  return 0;
}
