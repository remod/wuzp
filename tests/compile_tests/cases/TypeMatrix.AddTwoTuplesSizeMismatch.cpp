// wuzp-expect: error
// wuzp-static-assert: Tuple sizes do not match\.

// Only tuples of units of the same size can be summed elementwise.

#include "wuzp/core/TypeMatrix.hpp"

#include "Unit.hpp"

int main() {
  using Type = wuzp::AddTwoTuples<std::tuple<Length>, std::tuple<Length, Length>>::type;
  [[maybe_unused]] constexpr auto size = sizeof(Type);
  return 0;
}
