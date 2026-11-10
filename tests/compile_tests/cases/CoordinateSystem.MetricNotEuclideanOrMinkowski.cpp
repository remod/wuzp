// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// The metric of a coordinate system is Euclidean or Minkowski, not e.g. a number.

#include "wuzp/core/CoordinateSystem.hpp"

struct IdentifierA {};

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(wuzp::CoordinateSystem<IdentifierA, int>);
  return 0;
}
