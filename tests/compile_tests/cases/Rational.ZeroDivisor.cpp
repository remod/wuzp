// wuzp-expect: error
// wuzp-static-assert: The divisor must not be zero\.

#include "wuzp/core/Rational.hpp"

using namespace wuzp;

int main() {
  using type = Rational<3, 0>;
  return 0;
}
