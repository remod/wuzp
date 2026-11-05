// wuzp-expect: error
// wuzp-static-assert: Quantities are not homogeneous\.

// The squared norm requires all elements to have the same unit.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  using Heterogeneous = wuzp::Quantities<ValueMatrix<double, 1, 2>, wuzp::TypeMatrix<1, 2, Length, Time>>;
  const Heterogeneous quantities{1.0, 2.0};
  [[maybe_unused]] const auto norm = quantities.squaredNorm();
  return 0;
}
