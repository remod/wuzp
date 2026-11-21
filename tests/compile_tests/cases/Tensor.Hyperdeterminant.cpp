// wuzp-expect: error
// wuzp-static-assert: Not implemented

// The hyperdeterminant is not implemented. The result is used, like in user code: a rejecting function that returns void failed
// with MSVC before its static_assert, at the use of the result (#85).

#include "compile_test_types.hpp"

int main() {
  [[maybe_unused]] const auto hyperdeterminant = TestTensor<Indices<Up>>::zero().hyperdeterminant();
  return 0;
}
