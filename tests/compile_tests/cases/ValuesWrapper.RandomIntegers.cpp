// wuzp-expect: error
// wuzp-static-assert: Random values are only supported for floating point types\.

// Random values are uniformly distributed in [-1, 1], which is only meaningful for floating point types.

#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const auto values = ValueMatrix<int, 2, 2>::random();
  return 0;
}
