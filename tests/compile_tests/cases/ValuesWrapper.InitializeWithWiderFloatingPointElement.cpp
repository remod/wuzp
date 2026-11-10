// wuzp-expect: error
// wuzp-static-assert: The common type of the element and the value type is not the value type, e\.g\. a double element with int values\.

// A double element is not converted into float, the value type of the values, which could lose precision: values of floats are
// initialized with float elements, e.g. 0.5F.

#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const ValueMatrix<float, 2, 1> matrix{1.0F, 0.5};
  return 0;
}
