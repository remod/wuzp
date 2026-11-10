// wuzp-expect: error
// wuzp-static-assert: The common type of the element and the value type is not the value type, e\.g\. a double element with int values\.

// The value of a quantity is set like its elements: a double for int values is rejected, since it could be truncated.

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  QuantityI<Length> length{1};
  length.setValue(1.5);
  return 0;
}
