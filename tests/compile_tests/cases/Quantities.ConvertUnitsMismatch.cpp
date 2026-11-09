// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// A length cannot be converted into a time, not even explicitly. Direct initialization explains why the conversion fails,
// while copy initialization does not consider it (#70).

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  const QuantityD<Length> length{1.0};
  [[maybe_unused]] const QuantityD<Time> time{length};
  return 0;
}
