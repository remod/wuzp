// wuzp-expect: error
// wuzp-static-assert: Only a tensor with one upper index, a vector or a batch of vectors, can be absolute \(a point\)\.

// An absolute scalar, e.g. a time point, is a point in a one-dimensional coordinate system whose origin is its zero, e.g. an
// epoch: a tensor with one upper index, e.g. wuzp::TimePoint.

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] constexpr auto size = sizeof(TestTensor<Indices<>, wuzp::Absolute>);
  return 0;
}
