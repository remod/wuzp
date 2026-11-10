// wuzp-expect: error
// wuzp-static-assert: The inverse of an integer matrix is not an integer matrix\. Use a floating-point value type\.

// The core rejects the inverses of integer matrices with every value matrix backend: they are not integer matrices in general,
// and a backend would truncate them, e.g. Eigen divides integers.

#include "ValueMatrix.hpp"

int main() {
  const ValueMatrix<int, 2, 2> matrix{1, 2, 3, 4};
  [[maybe_unused]] const auto result = matrix.inverse();
  return 0;
}
