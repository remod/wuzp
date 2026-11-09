// wuzp-expect: error
// wuzp-static-assert: Not implemented

// The hyperdeterminant is not implemented.

#include "compile_test_types.hpp"

int main() {
  TestTensor<Indices<Up>>::zero().hyperdeterminant();
  return 0;
}
