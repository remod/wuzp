// wuzp-expect: error
// wuzp-static-assert: The common type of the element and the value type is not the value type, e\.g\. a double element with int values\.

// An element is converted into the value type of the values, e.g. an int element for double values, but not if this narrows
// it, like the scalars of operations: a double element would be truncated into int.

#include "ValueMatrix.hpp"

int main() {
  [[maybe_unused]] const ValueMatrix<int, 2, 1> matrix{1, 1.5};
  return 0;
}
