// wuzp-expect: error
// wuzp-static-assert: Tuple sizes do not match\.

// Only tuples of units of the same size can be multiplied elementwise.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::MultiplyTwoTuples<std::tuple<Length>, std::tuple<Length, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
