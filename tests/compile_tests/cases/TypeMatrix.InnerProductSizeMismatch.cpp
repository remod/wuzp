// wuzp-expect: error
// wuzp-static-assert: Matrix sizes do not match\.

// A matrix product requires the columns of the left matrix to match the rows of the right matrix.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::InnerProduct<wuzp::TypeMatrix<2, 3, Length>, wuzp::TypeMatrix<2, 3, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
