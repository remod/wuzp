// wuzp-expect: error
// wuzp-static-assert: Number of rows do not match\.

// Values only convert into values of the same size (#62).

#include "ValueMatrix.hpp"

int main() {
  const ValueMatrix<double, 2, 1> values{1.0, 2.0};
  [[maybe_unused]] const ValueMatrix<double, 3, 1> converted{values};
  return 0;
}
