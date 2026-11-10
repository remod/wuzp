// wuzp-expect: error
// wuzp-static-assert: The common type of the element and the value type is not the value type, e\.g\. a double element with int values\.

// An element is set like the elements of the constructor: it is converted into the value type, e.g. an int element for double
// values, but not if this could truncate it. Quantities and tensors set their elements with it.

#include "ValueMatrix.hpp"

int main() {
  Value<int> value{1};
  value.set(1.5);
  return 0;
}
