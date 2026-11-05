// wuzp-expect: error
// wuzp-requires: values=blitz
// wuzp-static-assert: Inverse is not implemented for blitz\.

// Blitz++ does not provide a matrix inverse.

#include "ValueMatrix.hpp"

int main() {
  const ValueMatrix<double, 2, 2> matrix{1.0, 2.0, 3.0, 4.0};
  [[maybe_unused]] const auto result = matrix.inverse();
  return 0;
}
