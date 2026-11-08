// wuzp-expect: error
// wuzp-static-assert: Number of cols do not match\.

// Values only convert into values of the same size (#62).

#include "ValueMatrix.hpp"

int main() {
  const ValueMatrix<double, 1, 2> values{1.0, 2.0};
  [[maybe_unused]] const ValueMatrix<double, 1, 3> converted{values};
  return 0;
}
