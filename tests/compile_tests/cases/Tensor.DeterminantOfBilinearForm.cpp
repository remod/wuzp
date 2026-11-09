// wuzp-expect: error
// wuzp-static-assert: The determinant of a tensor with two upper or two lower indices depends on the basis\. Use getQuantities\(\)\.determinant\(\) for the determinant of its components\.

// The determinant of a tensor with two lower indices, e.g. of a mass matrix, changes by det(A)^-2 under a linear transformation
// A: it is a scalar density, not a scalar.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Down, Down>>::zero().determinant();
  return 0;
}
