// wuzp-expect: error
// wuzp-static-assert: 'AccessValues' trait not specialized for given type\. Make sure you included the necessary value headers\.

// Using quantities without including a value matrix backend (e.g. wuzp_eigen/ValueMatrix.hpp) tells the user what is missing.

#include "wuzp/core/Quantity.hpp"

#include "Unit.hpp"

int main() {
  [[maybe_unused]] const wuzp::QuantityD<Length> length{1.0};
  return 0;
}
