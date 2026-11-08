// wuzp-expect: error
// wuzp-error: constraints?( are)? (failure|not satisfied)

// A passive transformation transforms from a coordinate system, not from the identifier of one.

#include "wuzp/trafo/PassiveTransformation.hpp"

#include "coordinate_systems.hpp"

template <typename From, typename To>
class MyTransformation : public wuzp::PassiveTransformation<MyTransformation<From, To>, From, To> {};

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(MyTransformation<CoordinateSystemAId, CoordinateSystemB>);
  return 0;
}
