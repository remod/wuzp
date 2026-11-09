// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// Whether a tensor is a point or a vector is given by wuzp::Absolute or wuzp::Relative, not e.g. by a bool.

#include <type_traits>

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(TestTensor<Indices<Up>, std::true_type>);
  return 0;
}
