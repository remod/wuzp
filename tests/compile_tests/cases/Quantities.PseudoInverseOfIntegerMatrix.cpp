// wuzp-expect: error
// wuzp-static-assert: The inverse of an integer matrix is not an integer matrix\. Use a floating-point value type\.

// The pseudo-inverse needs a matrix inverse, which an integer matrix does not have in general (#77).

#include "wuzp/core/Quantities.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::Quantities<ValueMatrix<int, 2, 3>, wuzp::TypeMatrix<2, 3, Length>> matrix{1, 2, 3, 4, 5, 7};
  [[maybe_unused]] const auto pseudoInverse = matrix.pseudoInverse();
  return 0;
}
