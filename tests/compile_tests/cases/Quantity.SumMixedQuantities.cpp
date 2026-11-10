// wuzp-expect: error
// wuzp-static-assert: Units do not match\.

// The core checks that the units match, with every units backend (see wuzp/core/Unit.hpp).

#include "wuzp/core/Quantities.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

using namespace wuzp;  // NOLINT

using TimeQuantity = QuantityD<Time>;
using LengthQuantity = QuantityD<Length>;

int main() {
  [[maybe_unused]] const auto invalid = TimeQuantity{2.0} + LengthQuantity{3.0};
  return 0;
}
