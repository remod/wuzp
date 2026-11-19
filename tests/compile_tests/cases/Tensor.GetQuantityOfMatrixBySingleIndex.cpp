// wuzp-expect: error
// wuzp-error: no matching (member |overloaded )?function

// Only the components of tensors of dimension 0 and 1, e.g. scalars, vectors, points and covectors, have a single index: the
// requires clause removes it from a matrix, whose components have a row and a column. The other overloads take two indices or
// none, so the call fails because of the constraint only. GCC, Clang and MSVC report it as a call without a matching overload,
// and name the constraint in the notes.

#include "compile_test_types.hpp"

int main() {
  using LinearMap = wuzp::Tensor<Indices<Up, Down>, wuzp::Quantities<ValueMatrix<double, 2, 2>, wuzp::TypeMatrix<2, 2, Length>>,
                                 CoordinateSystemA, wuzp::Relative>;
  [[maybe_unused]] const auto quantity = LinearMap::zero().getQuantity<0>();
  return 0;
}
