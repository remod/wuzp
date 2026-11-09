// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The coordinate system of a tensor must be a coordinate system, not e.g. a matrix of coordinate systems.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(TestTensor<Indices<Up>, wuzp::Relative, wuzp::TypeMatrix<1, 1, CoordinateSystemA>>);
  return 0;
}
