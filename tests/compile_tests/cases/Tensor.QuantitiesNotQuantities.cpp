// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The components of a tensor are quantities: values without units are not enough.

#include "wuzp/core/Tensor.hpp"
#include "wuzp/core/Absolute.hpp"

#include "ValueMatrix.hpp"
#include "coordinate_systems.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::Tensor<1, 0, ValueMatrix<double, 3, 1>, CoordinateSystemA, wuzp::Relative>);
  return 0;
}
