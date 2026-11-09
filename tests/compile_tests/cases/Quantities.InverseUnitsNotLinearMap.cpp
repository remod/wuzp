// wuzp-expect: error
// wuzp-static-assert: The units of the matrix do not describe a linear map \(M_ij = a_i / b_j\)\.

// The units [[m, m], [m, s]] are not of the form a_i / b_j, so the units of the inverse are not defined: the summands of the
// elements of M M^-1 would have different units.

#include "wuzp/core/Quantities.hpp"
#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"
#include "ValueMatrix.hpp"

int main() {
  const wuzp::Quantities<ValueMatrix<double, 2, 2>, wuzp::TypeMatrix<2, 2, Length, Length, Length, Time>> quantities{1.0, 2.0, 3.0, 4.0};
  [[maybe_unused]] const auto inverse = quantities.inverse();
  return 0;
}
