// wuzp-expect: error
// wuzp-static-assert: The function must map a value of the value type to a value of the value type\.

// The values of a function applied to every element keep the value type: a function of int values that returns a double would
// give values of another value type than the values claim.

#include "ValueMatrix.hpp"

int main() {
  const ValueMatrix<int, 2, 1> values{1, 2};
  [[maybe_unused]] const auto result = values.cwiseFunction([](const int value) { return 0.5 * value; });
  return 0;
}
