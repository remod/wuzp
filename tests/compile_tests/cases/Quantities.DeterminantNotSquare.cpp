// wuzp-expect: error
// wuzp-static-assert: Matrix is not square\.

// Only square matrices have a determinant.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  const wuzp::Quantities<ValueMatrix<double, 2, 3>, wuzp::TypeMatrix<2, 3, Length>> quantities{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  [[maybe_unused]] const auto determinant = quantities.determinant();
  return 0;
}
