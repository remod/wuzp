// wuzp-expect: error
// wuzp-static-assert: A covector, a tensor with one lower index, is stored as a row\.

// A covector is stored as a row, so that the shape of a tensor follows from its indices.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size =
      sizeof(wuzp::Tensor<Indices<Down>, wuzp::Quantities<ValueMatrix<double, 3, 1>, wuzp::TypeMatrix<3, 1, Length>>, CoordinateSystemA,
                          wuzp::Relative>);
  return 0;
}
