// wuzp-expect: error
// wuzp-static-assert: An absolute object cannot be inverted\.

// The components of a point depend on the origin, so their inverse is not defined.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto result = TestTensor<Indices<Up>, wuzp::Absolute>::zero().inverse();
  return 0;
}
