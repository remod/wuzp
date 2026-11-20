// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// The pseudo-inverse (A^T A)^-1 A^T of a matrix with more rows than columns sums over its rows, which must have the same unit:
// otherwise the pseudo-inverse depends on the choice of units (#77).

#include "wuzp/core/Quantities.hpp"

#include "compile_test_types.hpp"

int main() {
  const wuzp::Quantities<ValueMatrix<double, 3, 2>, wuzp::TypeMatrix<3, 2, Length, Length, Time, Time, Length, Length>> matrix{
      1.0, 2.0, 3.0, 4.0, 5.0, 7.0};
  [[maybe_unused]] const auto pseudoInverse = matrix.pseudoInverse();
  return 0;
}
