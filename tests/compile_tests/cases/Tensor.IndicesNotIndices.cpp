// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The indices of a tensor are given as wuzp::Indices, e.g. Indices<Up, Down>, not e.g. as a tuple.

#include <tuple>

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(TestTensor<std::tuple<Up>>);
  return 0;
}
