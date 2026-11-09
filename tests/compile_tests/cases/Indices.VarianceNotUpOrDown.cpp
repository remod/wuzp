// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// Each index is upper or lower, wuzp::Up or wuzp::Down, not e.g. a number of indices.

#include <type_traits>

#include "wuzp/core/Indices.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::Indices<std::integral_constant<int, 1>>);
  return 0;
}
