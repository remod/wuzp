// wuzp-expect: error
// wuzp-static-assert: The vector of a skew-symmetric matrix is only defined for three-dimensional linear maps, tensors with an upper and a lower index\.

// The skew-symmetric matrix of a vector is a linear map, not a bilinear form (#77).

#include "compile_test_types.hpp"

int main() {
  using BilinearForm = wuzp::Tensor<Indices<Down, Down>, wuzp::Quantities<ValueMatrix<double, 3, 3>, wuzp::TypeMatrix<3, 3, Length>>,
                                    CoordinateSystemA, wuzp::Relative>;
  const BilinearForm form{0.0, -3.0, 2.0, 3.0, 0.0, -1.0, -2.0, 1.0, 0.0};
  [[maybe_unused]] const auto vector = wuzp::vectorOfSkewSymmetricMatrix(form);
  return 0;
}
